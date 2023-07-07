
#include "main.h"
#include <sstream>
#include <chrono>

void TSCpp::Write(char value) {
	std::stringstream ss;
	ss << "[OUT] Writing character '" << value << "' / ";
	ss << std::hex << (int) value;
	
	Log.Add(ss.str());
	Serial.writeChar(value);
}

void TSCpp::Write(std::string value) {
	std::stringstream ss;
	ss << "[OUT] Writing string '" << value << "' / ";
	for (int i = 0; i < value.length(); i++) {
		std::string sep = i != value.length() ? ":" : "";
		ss << std::hex << (int) value[i];
		ss << sep;
	}

	Log.Add(ss.str());
	Serial.writeString(value.c_str());
}

uint32_t TSCpp::ReverseBytes(uint32_t i) {
	return  ((i >> 24) & 0xffU) | // move byte 3 to byte 0
		((i << 8) & 0xff0000U) | // move byte 1 to byte 2
		((i >> 8) & 0xff00U) | // move byte 2 to byte 1
		((i << 24) & 0xff000000U);
}

uint32_t TSCpp::Write(uint32_t value) {
	value = ReverseBytes(value);
	const char* pBuffer = (const char*)&value;
	Write(pBuffer[0]);
	Write(pBuffer[1]);
	Write(pBuffer[2]);
	Write(pBuffer[3]);
	return value;
}

void TSCpp::Write(uint16_t value) {
	Write((char)((value >> 8U) & 255U));
	Write((char)(value & 255U));
}

int TSCpp::Read() {
	char value;
	Serial.readChar(&value);

	std::stringstream ss;
	ss << "[IN] Recieved character '" << value << "' / ";
	ss << std::hex << (int) value;

	Log.Add(ss.str());
	return (int) value;
}

uint32_t TSCpp::Read32() {
	std::stringstream raw;

	//int raw[sizeof(uint32_t)];
	uint32_t value;

	int count = 0;
	std::string last;
	while (count < sizeof(value)) {
		if (CheckTimeout()) {
			return 0;
		}

		if (Serial.available() > 0) {
            char c = Read();

			// hacky fix for TS' unicode characters (ex. ffffffb9)
			std::stringstream stream;
			stream << std::hex << (int)c;
			std::string result(stream.str());
			if (result.length() > 2) {
				// hex is longer than 3 chars so probably the ffffff**
				raw << result[6];
				raw << result[7];
			}
			else if (result.length() == 1) {
				raw << "0" << result;
			}
			else {
				raw << result;
			}

			count++;
		}
	}

	value = std::stoul(raw.str(), 0, 16);

	std::stringstream ss;
	ss << "[IN] Recieved uint32 " << value << " / ";
	ss << std::hex << value;

	Log.Add(ss.str());
	return value;
}

void TSCpp::Comms(char c) {
    Log.Add("\"Encrypted\" comms begin");
    
    // skip the empty char TS puts before "encrypted" messages
	while (Serial.available() == 0) { /* Wait for the 2nd byte to be received (This will almost never happen) */ }

	// usually 1
    serialPayloadLength = Read();
	serialBytesRxTx = 2;
	serialReceiveStartTime = milis();

    Log.Add("\"Encrypted\" payload length: " + std::to_string(serialPayloadLength));
	
    // recv "encrypted" command
	while (Serial.available() > 0) {
		if (serialBytesRxTx < (serialPayloadLength + 2)) {
			serialPayload[serialBytesRxTx - 2] = Read();
			serialBytesRxTx++;
		}
		else {
            break;
		}
	}

    Log.Add("\"Encrypted\" comms recieve CRC");
    uint32_t incomingCrc = Read32();

    if (!CheckTimeout()) {
        uint32_t wantedCrc = CRC::Calculate(serialPayload, serialPayloadLength, CRC::CRC_32());
        if (incomingCrc == wantedCrc) {
            //CRC is correct. Process the command
            Log.Add("\"Encrypted\" comms CRC correct");
            SerialCommand();
            allowLegacy = false;
        }
        else {
            //CRC Error. Need to send an error message
            Log.Add("\"Encrypted\" comms CRC error");

            Log.Add("Wanted CRC: " + std::to_string(wantedCrc));
            Log.Add("Recieved CRC: " + std::to_string(incomingCrc));

            SendReturnCode(SERIAL_RC_CRC_ERR);
            FlushRXBuffer();
        }
    }
    else {
        Log.Add("\"Encrypted\" comms timeout");
        FlushRXBuffer();
		SendReturnCode(SERIAL_RC_TIMEOUT);
	}

    Log.Add("\"Encrypted\" comms end");
}

uint64_t TSCpp::milis() {
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	uint64_t milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	return milliseconds;
}

bool TSCpp::CheckTimeout() {
	return (milis() - serialReceiveStartTime) > SERIAL_TIMEOUT;
}

void TSCpp::SendReturnCode(char returnCode) {
	Write((uint16_t)sizeof(returnCode));
	Write(returnCode);
	Write(CRC::Calculate(&returnCode, sizeof(returnCode), CRC::CRC_32()));
}

void TSCpp::FlushRXBuffer() {
	std::stringstream ss;
	ss << "Flushing " << Serial.available() << " entries from RX buffer.";

	Log.Add(ss.str());
	Serial.flushReceiver();
}

void TSCpp::SerialCommand() {
    switch (serialPayload[0])
    {
    case 'A': // send x bytes of realtime values
        //generateLiveValues(0, LOG_ENTRY_SIZE);
        break;

    case 'b': // New EEPROM burn command to only burn a single page at a time 
        //if ((micros() > deferEEPROMWritesUntil)) { writeConfig(serialPayload[2]); } //Read the table number and perform burn. Note that byte 1 in the array is unused
        //else { BIT_SET(currentStatus.status4, BIT_STATUS4_BURNPENDING); }
        writeConfig(serialPayload[2]);

        SendReturnCode(SERIAL_RC_BURN_OK);
        break;

    case 'B': // Same as above, but for the comms compat mode. Slows down the burn rate and increases the defer time
        /*BIT_SET(currentStatus.status4, BIT_STATUS4_COMMS_COMPAT); //Force the compat mode
        deferEEPROMWritesUntil += (EEPROM_DEFER_DELAY / 4); //Add 25% more to the EEPROM defer time
        if ((micros() > deferEEPROMWritesUntil)) { writeConfig(serialPayload[2]); } //Read the table number and perform burn. Note that byte 1 in the array is unused
        else { BIT_SET(currentStatus.status4, BIT_STATUS4_BURNPENDING); }
        */
        SendReturnCode(SERIAL_RC_BURN_OK);
        break;

    case 'C': // test communications. This is used by Tunerstudio to see whether there is an ECU on a given serial port
        memcpy(serialPayload, testCommsResponse, sizeof(testCommsResponse));
        SendPayload(sizeof(testCommsResponse));
        break;

    case 'd': // Send a CRC32 hash of a given page
    {
        uint32_t crc = ReverseBytes(calculatePageCRC32(serialPayload[2]));

        serialPayload[0] = SERIAL_RC_OK;
        memcpy(&serialPayload[1], &crc, sizeof(crc));
        SendPayload(5);
        break;
    }

    case 'E': // receive command button commands
        //(void)TS_CommandButtonsHandler(word(serialPayload[1], serialPayload[2]));
        SendReturnCode(SERIAL_RC_OK);
        break;

    case 'f': //Send serial capability details
        serialPayload[0] = SERIAL_RC_OK;
        serialPayload[1] = 2; //Serial protocol version
        serialPayload[2] = highByte(121);
        serialPayload[3] = lowByte(121);
        serialPayload[4] = highByte(64);
        serialPayload[5] = lowByte(64);

        SendPayload(6);
        break;

    case 'F': // send serial protocol version
        memcpy(serialPayload, serialVersion, sizeof(serialVersion));
        SendPayload(sizeof(serialVersion));
        break;

    case 'H': //Start the tooth logger
        //startToothLogger();
        SendReturnCode(SERIAL_RC_OK);
        break;

    case 'h': //Stop the tooth logger
        //stopToothLogger();
        SendReturnCode(SERIAL_RC_OK);
        break;

    case 'I': // send CAN ID
        memcpy(serialPayload, canId, sizeof(canId));
        SendPayload(sizeof(canId));
        break;

    case 'J': //Start the composite logger
        //startCompositeLogger();
        SendReturnCode(SERIAL_RC_OK);
        break;

    case 'j': //Stop the composite logger
        //stopCompositeLogger();
        SendReturnCode(SERIAL_RC_OK);
        break;

    case 'k': //Send CRC values for the calibration pages
    {
        /*uint32_t CRC32_val = reverse_bytes(readCalibrationCRC32(serialPayload[2])); //Get the CRC for the requested page

        serialPayload[0] = SERIAL_RC_OK;
        (void)memcpy(&serialPayload[1], &CRC32_val, sizeof(CRC32_val));
        sendSerialPayloadNonBlocking(5);*/
        break;
    }

    case 'M':
    {
        //New write command
        //7 bytes required:
        //2 - Page identifier
        //2 - offset
        //2 - Length
        //1 - 1st New value
        //SendReturnCode(SERIAL_RC_OK);
        //setPageValue(serialPayload[2], makeWord(serialPayload[4], serialPayload[3]), serialPayload, makeWord(serialPayload[6], serialPayload[5]), 7);

        if (updatePageValues(serialPayload[2], makeWord(serialPayload[4], serialPayload[3]), &serialPayload[7], makeWord(serialPayload[6], serialPayload[5])))
        {
            SendReturnCode(SERIAL_RC_OK);
        }
        else
        {
            //This should never happen, but just in case
            SendReturnCode(SERIAL_RC_RANGE_ERR);
        }
        break;
    }

    case 'O': //Start the composite logger 2nd cam (teritary)
        //startCompositeLoggerTertiary();
        SendReturnCode(SERIAL_RC_OK);
        break;

    case 'o': //Stop the composite logger 2nd cam (tertiary)
        //stopCompositeLoggerTertiary();
        SendReturnCode(SERIAL_RC_OK);
        break;

    case 'X': //Start the composite logger 2nd cam (teritary)
        //startCompositeLoggerCams();
        SendReturnCode(SERIAL_RC_OK);
        break;

    case 'x': //Stop the composite logger 2nd cam (tertiary)
        //stopCompositeLoggerCams();
        SendReturnCode(SERIAL_RC_OK);
        break;

        /*
        * New method for sending page values (MS command equivalent is 'r')
        */
        // Requested after 'd'
    case 'p':
    {
        //6 bytes required:
        //2 - Page identifier
        //2 - offset
        //2 - Length
        uint16_t length = makeWord(serialPayload[6], serialPayload[5]);

        //Setup the transmit buffer
        serialPayload[0] = SERIAL_RC_OK;
        loadPageValuesToBuffer(serialPayload[2], makeWord(serialPayload[4], serialPayload[3]), serialPayload, length, 1);
        SendPayload(length + 1U);
        break;
    }

    case 'Q': // send code version
        memcpy(serialPayload, codeVersion, sizeof(codeVersion));
        SendPayload(sizeof(codeVersion));
        break;

    case 'r': //New format for the optimised OutputChannels
    {
        uint8_t cmd = serialPayload[2];
        uint16_t offset = makeWord(serialPayload[4], serialPayload[3]);
        uint16_t length = makeWord(serialPayload[6], serialPayload[5]);

        if (cmd == 48U) //Send output channels command 0x30 is 48dec
        {
            generateLiveValues(offset, length);
            SendPayload(length + 1U);
        }
        else if (cmd == 0x0f)
        {
            //Request for signature
            memcpy(serialPayload, codeVersion, sizeof(codeVersion));
            SendPayload(sizeof(codeVersion));
        }
        else
        {
            //No other r/ commands should be called
        }
        break;
    }

    case 'S': // send code version
        memcpy(serialPayload, productString, sizeof(productString));
        SendPayload(sizeof(productString));
        //currentStatus.secl = 0; //This is required in TS3 due to its stricter timings
        break;

    case 'T': //Send 256 tooth log entries to Tuner Studios tooth logger
        /*logItemsTransmitted = 0;
        if (currentStatus.toothLogEnabled == true) { sendToothLog(); } //Sends tooth log values as ints
        else if (currentStatus.compositeTriggerUsed > 0) { sendCompositeLog(); }
        else { /* MISRA no-op }*/
        break;

    case 't': // receive new Calibration info. Command structure: "t", <tble_idx> <data array>.
    {
        /*uint8_t cmd = serialPayload[2];
        uint16_t offset = word(serialPayload[3], serialPayload[4]);
        uint16_t calibrationLength = word(serialPayload[5], serialPayload[6]); // Should be 256

        if (cmd == O2_CALIBRATION_PAGE)
        {
            loadO2CalibrationChunk(offset, calibrationLength);
            sendReturnCodeMsg(SERIAL_RC_OK);
            Serial.flush(); //This is safe because engine is assumed to not be running during calibration
        }
        else if (cmd == IAT_CALIBRATION_PAGE)
        {
            processTemperatureCalibrationTableUpdate(calibrationLength, IAT_CALIBRATION_PAGE, iatCalibration_values, iatCalibration_bins);
        }
        else if (cmd == CLT_CALIBRATION_PAGE)
        {
            processTemperatureCalibrationTableUpdate(calibrationLength, CLT_CALIBRATION_PAGE, cltCalibration_values, cltCalibration_bins);
        }
        else
        {
            sendReturnCodeMsg(SERIAL_RC_RANGE_ERR);
        }*/
        break;
    }

    case 'U': //User wants to reset the Arduino (probably for FW update)
        Write("Reset control is currently disabled.");
        break;

    case 'w':
    {
/*#ifdef RTC_ENABLED
        uint8_t cmd = serialPayload[2];
        uint16_t SD_arg1 = word(serialPayload[3], serialPayload[4]);
        uint16_t SD_arg2 = word(serialPayload[5], serialPayload[6]);
        if (cmd == SD_READWRITE_PAGE)
        {
            if ((SD_arg1 == SD_WRITE_DO_ARG1) && (SD_arg2 == SD_WRITE_DO_ARG2))
            {
                /*
                SD DO command. Single byte of data where the commands are:
                0 Reset
                1 Reset
                2 Stop logging
                3 Start logging
                4 Load status variable
                5 Init SD card
                
                uint8_t command = serialPayload[7];
                if (command == 2) { endSDLogging(); manualLogActive = false; }
                else if (command == 3) { beginSDLogging(); manualLogActive = true; }
                else if (command == 4) { setTS_SD_status(); }
                //else if(command == 5) { initSD(); }

                sendReturnCodeMsg(SERIAL_RC_OK);
            }
            else if ((SD_arg1 == SD_WRITE_DIR_ARG1) && (SD_arg2 == SD_WRITE_DIR_ARG2))
            {
                //Begin SD directory read. Value in payload represents the directory chunk to read
                //Directory chunks are each 16 files long
                SDcurrentDirChunk = word(serialPayload[7], serialPayload[8]);
                sendReturnCodeMsg(SERIAL_RC_OK);
            }
            else if ((SD_arg1 == SD_WRITE_SEC_ARG1) && (SD_arg2 == SD_WRITE_SEC_ARG2))
            {
                //SD write sector command
            }
            else if ((SD_arg1 == SD_ERASEFILE_ARG1) && (SD_arg2 == SD_ERASEFILE_ARG2))
            {
                //Erase file command
                //We just need the 4 ASCII characters of the file name
                char log1 = serialPayload[7];
                char log2 = serialPayload[8];
                char log3 = serialPayload[9];
                char log4 = serialPayload[10];

                deleteLogFile(log1, log2, log3, log4);
                sendReturnCodeMsg(SERIAL_RC_OK);
            }
            else if ((SD_arg1 == SD_SPD_TEST_ARG1) && (SD_arg2 == SD_SPD_TEST_ARG2))
            {
                //Perform a speed test on the SD card
                //First 4 bytes are the sector number to write to
                uint32_t sector;
                uint8_t sector1 = serialPayload[7];
                uint8_t sector2 = serialPayload[8];
                uint8_t sector3 = serialPayload[9];
                uint8_t sector4 = serialPayload[10];
                sector = (sector1 << 24) | (sector2 << 16) | (sector3 << 8) | sector4;


                //Last 4 bytes are the number of sectors to test
                uint32_t testSize;
                uint8_t testSize1 = serialPayload[11];
                uint8_t testSize2 = serialPayload[12];
                uint8_t testSize3 = serialPayload[13];
                uint8_t testSize4 = serialPayload[14];
                testSize = (testSize1 << 24) | (testSize2 << 16) | (testSize3 << 8) | testSize4;

                sendReturnCodeMsg(SERIAL_RC_OK);

            }
            else if ((SD_arg1 == SD_WRITE_COMP_ARG1) && (SD_arg2 == SD_WRITE_COMP_ARG2))
            {
                //Prepare to read a 2024 byte chunk of data from the SD card
                uint8_t sector1 = serialPayload[7];
                uint8_t sector2 = serialPayload[8];
                uint8_t sector3 = serialPayload[9];
                uint8_t sector4 = serialPayload[10];
                //SDreadStartSector = (sector1 << 24) | (sector2 << 16) | (sector3 << 8) | sector4;
                SDreadStartSector = (sector4 << 24) | (sector3 << 16) | (sector2 << 8) | sector1;
                //SDreadStartSector = sector4 | (sector3 << 8) | (sector2 << 16) | (sector1 << 24);

                //Next 4 bytes are the number of sectors to write
                uint8_t sectorCount1 = serialPayload[11];
                uint8_t sectorCount2 = serialPayload[12];
                uint8_t sectorCount3 = serialPayload[13];
                uint8_t sectorCount4 = serialPayload[14];
                SDreadNumSectors = (sectorCount1 << 24) | (sectorCount2 << 16) | (sectorCount3 << 8) | sectorCount4;

                //Reset the sector counter
                SDreadCompletedSectors = 0;

                sendReturnCodeMsg(SERIAL_RC_OK);
            }
        }
        else if (cmd == SD_RTC_PAGE)
        {
            //Used for setting RTC settings
            if ((SD_arg1 == SD_RTC_WRITE_ARG1) && (SD_arg2 == SD_RTC_WRITE_ARG2))
            {
                //Set the RTC date/time
                byte second = serialPayload[7];
                byte minute = serialPayload[8];
                byte hour = serialPayload[9];
                //byte dow = serialPayload[10]; //Not used
                byte day = serialPayload[11];
                byte month = serialPayload[12];
                uint16_t year = word(serialPayload[13], serialPayload[14]);
                rtc_setTime(second, minute, hour, day, month, year);
                sendReturnCodeMsg(SERIAL_RC_OK);
            }
        }
#endif*/
        break;
    }

    default:
        //Unknown command
        SendReturnCode(SERIAL_RC_UKWN_ERR);
        break;
    }
}

void TSCpp::SendPayload(uint16_t size) {
    Log.Add("\"Encrypted\" sending payload length: " + std::to_string(size));
    Write(size);
    SendBufferAndCRC(size);
}

uint32_t crctbl[256];

void gentbl(void)
{
    uint32_t crc;
    uint32_t c;
    uint32_t i;
    for (c = 0; c < 0x100; c++) {
        crc = c;
        for (i = 0; i < 8; i++) {
            crc = (crc & 1) ? (crc >> 1) ^ 0xedb88320 : (crc >> 1);
        }
        crctbl[c] = crc;
    }
}

uint32_t crc32(uint8_t* bfr, size_t size)
{
    uint32_t crc = 0xfffffffful;
    while (size--)
        crc = (crc >> 8) ^ crctbl[(crc & 0xff) ^ *bfr++];
    return(crc ^ 0xfffffffful);
}

void TSCpp::SendBufferAndCRC(uint16_t size) {
    std::list<unsigned char> list;
    
    Log.Add("\"Encrypted\" sending payload..");
    for (int i = 0; i < size; i++) {
        Write((char)serialPayload[i]);
        list.push_back(serialPayload[i]);
    }

    int arsize = list.size();
    unsigned char* crcPayload = new unsigned char[arsize];

    std::copy(std::begin(list), std::end(list), crcPayload);

    //uint32_t crc = CRC::Calculate(crcPayload, arsize, CRC::CRC_32());
    gentbl();
    uint32_t crc = crc32(crcPayload, arsize);
    std::stringstream ss;
    ss << "\"Encrypted\" sending payload CRC... (";
    ss << std::hex << crc;
    ss << ")";
    Log.Add(ss.str());
    Write(crc);

    delete[] crcPayload;
}

uint16_t TSCpp::freeRam() {
    return 1;
}

// ported

void TSCpp::loadPageValuesToBuffer(uint8_t pageNum, uint16_t offset, char* buffer, uint16_t length, int bufferOffset) {
    for (uint16_t i = 0; i < length; i++) {
        buffer[bufferOffset + i] = getPageValue(pageNum, offset + i);
    }
}

void TSCpp::generateLiveValues(uint16_t offset, uint16_t packetLength)
{
    //currentStatus.spark ^= (-currentStatus.hasSync ^ currentStatus.spark) & (1U << BIT_SPARK_SYNC); //Set the sync bit of the Spark variable to match the hasSync variable
    currentStatus.spark ^= (-currentStatus.hasSync ^ currentStatus.spark) & (1U << 7); //Set the sync bit of the Spark variable to match the hasSync variable

    serialPayload[0] = SERIAL_RC_OK;
    for (byte x = 0; x < packetLength; x++)
    {
        serialPayload[x + 1] = getTSLogEntry(offset + x);
    }
}

byte TSCpp::getTSLogEntry(uint16_t byteNum)
{
    byte statusValue = 0;

    switch (byteNum)
    {
    case 0: statusValue = currentStatus.secl; break; //secl is simply a counter that increments each second. Used to track unexpected resets (Which will reset this count to 0)
    case 1: statusValue = currentStatus.status1; break; //status1 Bitfield
    case 2: statusValue = currentStatus.engine; break; //Engine Status Bitfield
    case 3: statusValue = currentStatus.syncLossCounter; break;
    case 4: statusValue = lowByte(currentStatus.MAP); break; //2 bytes for MAP
    case 5: statusValue = highByte(currentStatus.MAP); break;
    case 6: statusValue = (byte)(currentStatus.IAT + 0); break; //mat
    case 7: statusValue = (byte)(currentStatus.coolant + 0); break; //Coolant ADC
    case 8: statusValue = currentStatus.batCorrection; break; //Battery voltage correction (%)
    case 9: statusValue = currentStatus.battery10; break; //battery voltage
    case 10: statusValue = currentStatus.O2; break; //O2
    case 11: statusValue = currentStatus.egoCorrection; break; //Exhaust gas correction (%)
    case 12: statusValue = currentStatus.iatCorrection; break; //Air temperature Correction (%)
    case 13: statusValue = currentStatus.wueCorrection; break; //Warmup enrichment (%)
    case 14: statusValue = lowByte(currentStatus.RPM); break; //rpm HB
    case 15: statusValue = highByte(currentStatus.RPM); break; //rpm LB
    case 16: statusValue = (byte)(currentStatus.AEamount >> 1); break; //TPS acceleration enrichment (%) divided by 2 (Can exceed 255)
    case 17: statusValue = lowByte(currentStatus.corrections); break; //Total GammaE (%)
    case 18: statusValue = highByte(currentStatus.corrections); break; //Total GammaE (%)
    case 19: statusValue = currentStatus.VE1; break; //VE 1 (%)
    case 20: statusValue = currentStatus.VE2; break; //VE 2 (%)
    case 21: statusValue = currentStatus.afrTarget; break;
    case 22: statusValue = lowByte(currentStatus.tpsDOT); break; //TPS DOT
    case 23: statusValue = highByte(currentStatus.tpsDOT); break; //TPS DOT
    case 24: statusValue = currentStatus.advance; break;
    case 25: statusValue = currentStatus.TPS; break; // TPS (0% to 100%)

    case 26:
        if (currentStatus.loopsPerSecond > 60000) { currentStatus.loopsPerSecond = 60000; }
        statusValue = lowByte(currentStatus.loopsPerSecond);
        break;
    case 27:
        if (currentStatus.loopsPerSecond > 60000) { currentStatus.loopsPerSecond = 60000; }
        statusValue = highByte(currentStatus.loopsPerSecond);
        break;

    case 28:
        currentStatus.freeRAM = freeRam();
        statusValue = lowByte(currentStatus.freeRAM); //(byte)((currentStatus.loopsPerSecond >> 8) & 0xFF);
        break;
    case 29:
        currentStatus.freeRAM = freeRam();
        statusValue = highByte(currentStatus.freeRAM);
        break;

    case 30: statusValue = (byte)(currentStatus.boostTarget >> 1); break; //Divide boost target by 2 to fit in a byte
    case 31: statusValue = (byte)(currentStatus.boostDuty / 100); break;
    case 32: statusValue = currentStatus.spark; break; //Spark related bitfield

        //rpmDOT must be sent as a signed integer
    case 33: statusValue = lowByte(currentStatus.rpmDOT); break;
    case 34: statusValue = highByte(currentStatus.rpmDOT); break;

    case 35: statusValue = currentStatus.ethanolPct; break; //Flex sensor value (or 0 if not used)
    case 36: statusValue = currentStatus.flexCorrection; break; //Flex fuel correction (% above or below 100)
    case 37: statusValue = currentStatus.flexIgnCorrection; break; //Ignition correction (Increased degrees of advance) for flex fuel

    case 38: statusValue = currentStatus.idleLoad; break;
    case 39: statusValue = currentStatus.testOutputs; break;

    case 40: statusValue = currentStatus.O2_2; break; //O2
    case 41: statusValue = currentStatus.baro; break; //Barometer value

    case 42: statusValue = lowByte(currentStatus.canin[0]); break;
    case 43: statusValue = highByte(currentStatus.canin[0]); break;
    case 44: statusValue = lowByte(currentStatus.canin[1]); break;
    case 45: statusValue = highByte(currentStatus.canin[1]); break;
    case 46: statusValue = lowByte(currentStatus.canin[2]); break;
    case 47: statusValue = highByte(currentStatus.canin[2]); break;
    case 48: statusValue = lowByte(currentStatus.canin[3]); break;
    case 49: statusValue = highByte(currentStatus.canin[3]); break;
    case 50: statusValue = lowByte(currentStatus.canin[4]); break;
    case 51: statusValue = highByte(currentStatus.canin[4]); break;
    case 52: statusValue = lowByte(currentStatus.canin[5]); break;
    case 53: statusValue = highByte(currentStatus.canin[5]); break;
    case 54: statusValue = lowByte(currentStatus.canin[6]); break;
    case 55: statusValue = highByte(currentStatus.canin[6]); break;
    case 56: statusValue = lowByte(currentStatus.canin[7]); break;
    case 57: statusValue = highByte(currentStatus.canin[7]); break;
    case 58: statusValue = lowByte(currentStatus.canin[8]); break;
    case 59: statusValue = highByte(currentStatus.canin[8]); break;
    case 60: statusValue = lowByte(currentStatus.canin[9]); break;
    case 61: statusValue = highByte(currentStatus.canin[9]); break;
    case 62: statusValue = lowByte(currentStatus.canin[10]); break;
    case 63: statusValue = highByte(currentStatus.canin[10]); break;
    case 64: statusValue = lowByte(currentStatus.canin[11]); break;
    case 65: statusValue = highByte(currentStatus.canin[11]); break;
    case 66: statusValue = lowByte(currentStatus.canin[12]); break;
    case 67: statusValue = highByte(currentStatus.canin[12]); break;
    case 68: statusValue = lowByte(currentStatus.canin[13]); break;
    case 69: statusValue = highByte(currentStatus.canin[13]); break;
    case 70: statusValue = lowByte(currentStatus.canin[14]); break;
    case 71: statusValue = highByte(currentStatus.canin[14]); break;
    case 72: statusValue = lowByte(currentStatus.canin[15]); break;
    case 73: statusValue = highByte(currentStatus.canin[15]); break;

    case 74: statusValue = currentStatus.tpsADC; break;
    //case 75: statusValue = getNextError(); break;
    case 75: statusValue = 0; break;

    case 76: statusValue = lowByte(currentStatus.PW1); break; //Pulsewidth 1 multiplied by 10 in ms. Have to convert from uS to mS.
    case 77: statusValue = highByte(currentStatus.PW1); break; //Pulsewidth 1 multiplied by 10 in ms. Have to convert from uS to mS.
    case 78: statusValue = lowByte(currentStatus.PW2); break; //Pulsewidth 2 multiplied by 10 in ms. Have to convert from uS to mS.
    case 79: statusValue = highByte(currentStatus.PW2); break; //Pulsewidth 2 multiplied by 10 in ms. Have to convert from uS to mS.
    case 80: statusValue = lowByte(currentStatus.PW3); break; //Pulsewidth 3 multiplied by 10 in ms. Have to convert from uS to mS.
    case 81: statusValue = highByte(currentStatus.PW3); break; //Pulsewidth 3 multiplied by 10 in ms. Have to convert from uS to mS.
    case 82: statusValue = lowByte(currentStatus.PW4); break; //Pulsewidth 4 multiplied by 10 in ms. Have to convert from uS to mS.
    case 83: statusValue = highByte(currentStatus.PW4); break; //Pulsewidth 4 multiplied by 10 in ms. Have to convert from uS to mS.

    case 84: statusValue = currentStatus.status3; break;
    case 85: statusValue = currentStatus.engineProtectStatus; break;
    case 86: statusValue = lowByte(currentStatus.fuelLoad); break;
    case 87: statusValue = highByte(currentStatus.fuelLoad); break;
    case 88: statusValue = lowByte(currentStatus.ignLoad); break;
    case 89: statusValue = highByte(currentStatus.ignLoad); break;
    case 90: statusValue = lowByte(currentStatus.dwell); break;
    case 91: statusValue = highByte(currentStatus.dwell); break;
    case 92: statusValue = currentStatus.CLIdleTarget; break;
    case 93: statusValue = lowByte(currentStatus.mapDOT); break;
    case 94: statusValue = highByte(currentStatus.mapDOT); break;
    case 95: statusValue = lowByte(currentStatus.vvt1Angle); break; //2 bytes for vvt1Angle
    case 96: statusValue = highByte(currentStatus.vvt1Angle); break;
    case 97: statusValue = currentStatus.vvt1TargetAngle; break;
    case 98: statusValue = (byte)(currentStatus.vvt1Duty); break;
    case 99: statusValue = lowByte(currentStatus.flexBoostCorrection); break;
    case 100: statusValue = highByte(currentStatus.flexBoostCorrection); break;
    case 101: statusValue = currentStatus.baroCorrection; break;
    case 102: statusValue = currentStatus.VE; break; //Current VE (%). Can be equal to VE1 or VE2 or a calculated value from both of them
    case 103: statusValue = currentStatus.ASEValue; break; //Current ASE (%)
    case 104: statusValue = lowByte(currentStatus.vss); break;
    case 105: statusValue = highByte(currentStatus.vss); break;
    case 106: statusValue = currentStatus.gear; break;
    case 107: statusValue = currentStatus.fuelPressure; break;
    case 108: statusValue = currentStatus.oilPressure; break;
    case 109: statusValue = currentStatus.wmiPW; break;
    case 110: statusValue = currentStatus.status4; break;
    case 111: statusValue = lowByte(currentStatus.vvt2Angle); break; //2 bytes for vvt2Angle
    case 112: statusValue = highByte(currentStatus.vvt2Angle); break;
    case 113: statusValue = currentStatus.vvt2TargetAngle; break;
    case 114: statusValue = (byte)(currentStatus.vvt2Duty); break;
    case 115: statusValue = currentStatus.outputsStatus; break;
    case 116: statusValue = (byte)(currentStatus.fuelTemp + 0); break; //Fuel temperature from flex sensor
    case 117: statusValue = currentStatus.fuelTempCorrection; break; //Fuel temperature Correction (%)
    case 118: statusValue = currentStatus.advance1; break; //advance 1 (%)
    case 119: statusValue = currentStatus.advance2; break; //advance 2 (%)
    case 120: statusValue = currentStatus.TS_SD_Status; break; //SD card status
    case 121: statusValue = lowByte(currentStatus.EMAP); break; //2 bytes for EMAP
    case 122: statusValue = highByte(currentStatus.EMAP); break;
    case 123: statusValue = currentStatus.fanDuty; break;
    case 124: statusValue = currentStatus.airConStatus; break;
    case 125: statusValue = lowByte(currentStatus.actualDwell); break;
    case 126: statusValue = highByte(currentStatus.actualDwell); break;
    }

    return statusValue;
}

bool TSCpp::updatePageValues(uint8_t pageNum, uint16_t offset, const char* buffer, uint16_t length)
{
    if ((offset + length) <= getPageSize(pageNum))
    {
        for (uint16_t i = 0; i < length; i++)
        {
            setPageValue(pageNum, (offset + i), buffer[i]);
        }
        return true;
    }

    return false;
}
