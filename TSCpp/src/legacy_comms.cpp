
#include "main.h"

void TSCpp::LegacyComms(char c) {
    switch (c)
    {
    case 'a':
        /*serialStatusFlag = SERIAL_COMMAND_INPROGRESS_LEGACY;
        if (Serial.available() >= 2)
        {
            Serial.read(); //Ignore the first value, it's always 0
            Serial.read(); //Ignore the second value, it's always 6
            sendValuesLegacy();
            serialStatusFlag = SERIAL_INACTIVE;
        }*/
        break;

    case 'A': // send x bytes of realtime values
        //sendValues(0, LOG_ENTRY_SIZE, 0x31, 0);   //send values to serial0
        break;

    case 'b': // New EEPROM burn command to only burn a single page at a time
        /*serialStatusFlag = SERIAL_COMMAND_INPROGRESS_LEGACY;

        if (Serial.available() >= 2)
        {
            Serial.read(); //Ignore the first table value, it's always 0
            writeConfig(Serial.read());
            serialStatusFlag = SERIAL_INACTIVE;
        }*/
        break;

    case 'B': // AS above but for the serial compatibility mode. 
        /*serialStatusFlag = SERIAL_COMMAND_INPROGRESS_LEGACY;
        BIT_SET(currentStatus.status4, BIT_STATUS4_COMMS_COMPAT); //Force the compat mode

        if (Serial.available() >= 2)
        {
            Serial.read(); //Ignore the first table value, it's always 0
            writeConfig(Serial.read());
            serialStatusFlag = SERIAL_INACTIVE;
        }*/
        break;

    case 'C': // test communications. This is used by Tunerstudio to see whether there is an ECU on a given serial port
        Write((char)1);
        break;

    case 'c': //Send the current loops/sec value
        Write((char)lowByte(currentStatus.loopsPerSecond));
        Write((char)highByte(currentStatus.loopsPerSecond));
        break;

    case 'd': // Send a CRC32 hash of a given page
        /*serialStatusFlag = SERIAL_COMMAND_INPROGRESS_LEGACY;

        if (Serial.available() >= 2)
        {
            Serial.read(); //Ignore the first byte value, it's always 0
            uint32_t CRC32_val = calculatePageCRC32(Serial.read());

            //Split the 4 bytes of the CRC32 value into individual bytes and send
            Serial.write(((CRC32_val >> 24) & 255));
            Serial.write(((CRC32_val >> 16) & 255));
            Serial.write(((CRC32_val >> 8) & 255));
            Serial.write((CRC32_val & 255));

            serialStatusFlag = SERIAL_INACTIVE;
        }*/
        break;

    case 'E': // receive command button commands
        /*serialStatusFlag = SERIAL_COMMAND_INPROGRESS_LEGACY;

        if (Serial.available() >= 2)
        {
            byte cmdGroup = (byte)Serial.read();
            (void)TS_CommandButtonsHandler(word(cmdGroup, Serial.read()));
            serialStatusFlag = SERIAL_INACTIVE;
        }*/
        break;

    case 'F': // send serial protocol version
        Write("002");
        break;

        //The G/g commands are used for bulk reading and writing to the EEPROM directly. This is typically a non-user feature but will be incorporated into SpeedyLoader for anyone programming many boards at once
    case 'G': // Dumps the EEPROM values to serial
        /*
        //The format is 2 bytes for the overall EEPROM size, a comma and then a raw dump of the EEPROM values
        Serial.write(lowByte(getEEPROMSize()));
        Serial.write(highByte(getEEPROMSize()));
        Serial.print(',');

        for (uint16_t x = 0; x < getEEPROMSize(); x++)
        {
            Serial.write(EEPROMReadRaw(x));
        }
        serialStatusFlag = SERIAL_INACTIVE;*/
        break;

    case 'g': // Receive a dump of raw EEPROM values from the user
    {
        //Format is similar to the above command. 2 bytes for the EEPROM size that is about to be transmitted, a comma and then a raw dump of the EEPROM values
        /*while (Serial.available() < 3) { delay(1); }
        uint16_t eepromSize = word(Serial.read(), Serial.read());
        if (eepromSize != getEEPROMSize())
        {
            //Client is trying to send the wrong EEPROM size. Don't let it
            Serial.println(F("ERR; Incorrect EEPROM size"));
            break;
        }
        else
        {
            for (uint16_t x = 0; x < eepromSize; x++)
            {
                while (Serial.available() < 3) { delay(1); }
                EEPROMWriteRaw(x, Serial.read());
            }
        }
        serialStatusFlag = SERIAL_INACTIVE;*/
        break;
    }

    case 'H': //Start the tooth logger
        //startToothLogger();
        //Serial.write(1); //TS needs an acknowledgement that this was received. I don't know if this is the correct response, but it seems to work
        break;

    case 'h': //Stop the tooth logger
        //stopToothLogger();
        break;

    case 'J': //Start the composite logger
        //startCompositeLogger();
        //Serial.write(1); //TS needs an acknowledgement that this was received. I don't know if this is the correct response, but it seems to work
        break;

    case 'j': //Stop the composite logger
        //stopCompositeLogger();
        break;

    case 'L': // List the contents of current page in human readable form
        //#ifndef SMALL_FLASH_MODE
        //                sendPageASCII();
        //#endif
        break;

    case 'm': //Send the current free memory
        currentStatus.freeRAM = freeRam();
        Write((char)currentStatus.freeRAM);
        Write((char)currentStatus.freeRAM);
        break;

    case 'N': // Displays a new line.  Like pushing enter in a text editor
        Write("\n");
        break;

    case 'O': //Start the composite logger 2nd cam (teritary)
        //startCompositeLoggerTertiary();
        //Serial.write(1); //TS needs an acknowledgement that this was received. I don't know if this is the correct response, but it seems to work
        break;

    case 'o': //Stop the composite logger 2nd cam (tertiary)
        //stopCompositeLoggerTertiary();
        break;

    case 'X': //Start the composite logger 2nd cam (teritary)
        //startCompositeLoggerCams();
        //Serial.write(1); //TS needs an acknowledgement that this was received. I don't know if this is the correct response, but it seems to work
        break;

    case 'x': //Stop the composite logger 2nd cam (tertiary)
        //stopCompositeLoggerCams();
        break;

    case 'P': // set the current page
        //This is a legacy function and is no longer used by TunerStudio. It is maintained for compatibility with other systems
        //A 2nd byte of data is required after the 'P' specifying the new page number.
        /*serialStatusFlag = SERIAL_COMMAND_INPROGRESS_LEGACY;

        if (Serial.available() > 0)
        {
            currentPage = Serial.read();
            //This converts the ASCII number char into binary. Note that this will break everything if there are ever more than 48 pages (48 = asci code for '0')
            if ((currentPage >= '0') && (currentPage <= '9')) // 0 - 9
            {
                currentPage -= 48;
            }
            else if ((currentPage >= 'a') && (currentPage <= 'f')) // 10 - 15
            {
                currentPage -= 87;
            }
            else if ((currentPage >= 'A') && (currentPage <= 'F'))
            {
                currentPage -= 55;
            }
            serialStatusFlag = SERIAL_INACTIVE;
        }*/
        break;

        /*
        * New method for sending page values
        */
    case 'p':
        /*serialStatusFlag = SERIAL_COMMAND_INPROGRESS_LEGACY;

        //6 bytes required:
        //2 - Page identifier
        //2 - offset
        //2 - Length
        if (Serial.available() >= 6)
        {
            byte offset1, offset2, length1, length2;
            int length;
            byte tempPage;

            Serial.read(); // First byte of the page identifier can be ignored. It's always 0
            tempPage = Serial.read();
            //currentPage = 1;
            offset1 = Serial.read();
            offset2 = Serial.read();
            valueOffset = word(offset2, offset1);
            length1 = Serial.read();
            length2 = Serial.read();
            length = word(length2, length1);
            for (int i = 0; i < length; i++)
            {
                Serial.write(getPageValue(tempPage, valueOffset + i));
            }

            serialStatusFlag = SERIAL_INACTIVE;
        }*/
        break;

    case 'Q': // send code version
        Write("speeduino 202305");
        break;

    case 'r': //New format for the optimised OutputChannels
        /*serialStatusFlag = SERIAL_COMMAND_INPROGRESS_LEGACY;
        byte cmd;
        if (Serial.available() >= 6)
        {
            Serial.read(); //Read the $tsCanId
            cmd = Serial.read(); // read the command

            uint16_t offset, length;
            byte tmp;
            tmp = Serial.read();
            offset = word(Serial.read(), tmp);
            tmp = Serial.read();
            length = word(Serial.read(), tmp);

            serialStatusFlag = SERIAL_INACTIVE;

            if (cmd == 0x30) //Send output channels command 0x30 is 48dec
            {
                sendValues(offset, length, cmd, 0);
            }
            else
            {
                //No other r/ commands are supported in legacy mode
            }
        }*/
        break;

    case 'S': // send code version
        Write("Speeduino 2023.05");
        break;

    case 'T': //Send 256 tooth log entries to Tuner Studios tooth logger
        //6 bytes required:
        //2 - Page identifier
        //2 - offset
        //2 - Length
        /*serialStatusFlag = SERIAL_COMMAND_INPROGRESS_LEGACY;
        if (Serial.available() >= 6)
        {
            Serial.read(); // First byte of the page identifier can be ignored. It's always 0
            Serial.read(); // First byte of the page identifier can be ignored. It's always 0
            Serial.read(); // First byte of the page identifier can be ignored. It's always 0
            Serial.read(); // First byte of the page identifier can be ignored. It's always 0
            Serial.read(); // First byte of the page identifier can be ignored. It's always 0
            Serial.read(); // First byte of the page identifier can be ignored. It's always 0

            if (currentStatus.toothLogEnabled == true) { sendToothLog_legacy(0); } //Sends tooth log values as ints
            else if (currentStatus.compositeTriggerUsed > 0) { sendCompositeLog_legacy(0); }
            serialStatusFlag = SERIAL_INACTIVE;
        }*/
        break;

    case 't': // receive new Calibration info. Command structure: "t", <tble_idx> <data array>.
        /*byte tableID;
        //byte canID;

        //The first 2 bytes sent represent the canID and tableID
        while (Serial.available() == 0) {}
        tableID = Serial.read(); //Not currently used for anything

        receiveCalibration(tableID); //Receive new values and store in memory
        writeCalibration(); //Store received values in EEPROM
        */
        break;

    case 'U': //User wants to reset the Arduino (probably for FW update)
        Write("Reset control is currently disabled.");
        break;

    case 'V': // send VE table and constants in binary
        //sendPage();
        break;

    case 'W': // receive new VE obr constant at 'W'+<offset>+<newbyte>
        /*serialStatusFlag = SERIAL_COMMAND_INPROGRESS_LEGACY;

        if (isMap())
        {
            if (Serial.available() >= 3) // 1 additional byte is required on the MAP pages which are larger than 255 bytes
            {
                byte offset1, offset2;
                offset1 = Serial.read();
                offset2 = Serial.read();
                valueOffset = word(offset2, offset1);
                setPageValue(currentPage, valueOffset, Serial.read());
                serialStatusFlag = SERIAL_INACTIVE;
            }
        }
        else
        {
            if (Serial.available() >= 2)
            {
                valueOffset = Serial.read();
                setPageValue(currentPage, valueOffset, Serial.read());
                serialStatusFlag = SERIAL_INACTIVE;
            }
        }*/

        break;

    case 'M':
        /*serialStatusFlag = SERIAL_COMMAND_INPROGRESS_LEGACY;

        if (chunkPending == false)
        {
            //This means it's a new request
            //7 bytes required:
            //2 - Page identifier
            //2 - offset
            //2 - Length
            //1 - 1st New value
            if (Serial.available() >= 7)
            {
                byte offset1, offset2, length1, length2;

                Serial.read(); // First byte of the page identifier can be ignored. It's always 0
                currentPage = Serial.read();
                //currentPage = 1;
                offset1 = Serial.read();
                offset2 = Serial.read();
                valueOffset = word(offset2, offset1);
                length1 = Serial.read();
                length2 = Serial.read();
                chunkSize = word(length2, length1);

                //Regular page data
                chunkPending = true;
                chunkComplete = 0;
            }
        }
        //This CANNOT be an else of the above if statement as chunkPending gets set to true above
        if (chunkPending == true)
        {
            while ((Serial.available() > 0) && (chunkComplete < chunkSize))
            {
                setPageValue(currentPage, (valueOffset + chunkComplete), Serial.read());
                chunkComplete++;
            }
            if (chunkComplete >= chunkSize) { serialStatusFlag = SERIAL_INACTIVE; chunkPending = false; }
        }*/
        break;

    case 'w':
        //No w commands are supported in legacy mode. This should never be called
        /*if (Serial.available() >= 7)
        {
            byte offset1, offset2, length1, length2;

            Serial.read(); // First byte of the page identifier can be ignored. It's always 0
            currentPage = Serial.read();
            //currentPage = 1;
            offset1 = Serial.read();
            offset2 = Serial.read();
            valueOffset = word(offset2, offset1);
            length1 = Serial.read();
            length2 = Serial.read();
            chunkSize = word(length2, length1);
        }*/
        break;

    case 'Z': //Totally non-standard testing function. Will be removed once calibration testing is completed. This function takes 1.5kb of program space! :S
        /*#ifndef SMALL_FLASH_MODE
                        Serial.println(F("Coolant"));
                        for (int x = 0; x < 32; x++)
                        {
                            Serial.print(cltCalibration_bins[x]);
                            Serial.print(", ");
                            Serial.println(cltCalibration_values[x]);
                        }
                        Serial.println(F("Inlet temp"));
                        for (int x = 0; x < 32; x++)
                        {
                            Serial.print(iatCalibration_bins[x]);
                            Serial.print(", ");
                            Serial.println(iatCalibration_values[x]);
                        }
                        Serial.println(F("O2"));
                        for (int x = 0; x < 32; x++)
                        {
                            Serial.print(o2Calibration_bins[x]);
                            Serial.print(", ");
                            Serial.println(o2Calibration_values[x]);
                        }
                        Serial.println(F("WUE"));
                        for (int x = 0; x < 10; x++)
                        {
                            Serial.print(configPage4.wueBins[x]);
                            Serial.print(F(", "));
                            Serial.println(configPage2.wueValues[x]);
                        }
                        Serial.flush();
        #endif*/
        break;

    case 'z': //Send 256 tooth log entries to a terminal emulator
        //sendToothLog_legacy(0); //Sends tooth log values as chars
        break;

    case '?':
        /*Serial.println
        (F(
            "\n"
            "===Command Help===\n\n"
            "All commands are single character and are concatenated with their parameters \n"
            "without spaces."
            "Syntax:  <command>+<parameter1>+<parameter2>+<parameterN>\n\n"
            "===List of Commands===\n\n"
            "A - Displays 31 bytes of currentStatus values in binary (live data)\n"
            "B - Burn current map and configPage values to eeprom\n"
            "C - Test COM port.  Used by Tunerstudio to see whether an ECU is on a given serial \n"
            "    port. Returns a binary number.\n"
            "N - Print new line.\n"
            "P - Set current page.  Syntax:  P+<pageNumber>\n"
            "R - Same as A command\n"
            "S - Display signature number\n"
            "Q - Same as S command\n"
            "V - Display map or configPage values in binary\n"
            "W - Set one byte in map or configPage.  Expects binary parameters. \n"
            "    Syntax:  W+<offset>+<newbyte>\n"
            "t - Set calibration values.  Expects binary parameters.  Table index is either 0, \n"
            "    1, or 2.  Syntax:  t+<tble_idx>+<newValue1>+<newValue2>+<newValueN>\n"
            "Z - Display calibration values\n"
            "T - Displays 256 tooth log entries in binary\n"
            "r - Displays 256 tooth log entries\n"
            "U - Prepare for firmware update. The next byte received will cause the Arduino to reset.\n"
            "? - Displays this help page"
        ));*/
        break;

    default:
        Write("Err: Unknown cmd");
        break;
    }
}
