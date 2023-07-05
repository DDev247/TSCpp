
#pragma once

#include "../../submodules/Serialib/lib/serialib.h"
#include <thread>
#include <CRC.h>

// include internal header files...
#include "logging.h"
#include "pages.h"
#include "eeprom.h"
#include "status.h"

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define SERIAL_RC_OK        0x00 //!< Success
#define SERIAL_RC_REALTIME  0x01 //!< Unused
#define SERIAL_RC_PAGE      0x02 //!< Unused

#define SERIAL_RC_BURN_OK   0x04 //!< EEPROM write succeeded

#define SERIAL_RC_TIMEOUT   0x80 //!< Timeout error
#define SERIAL_RC_CRC_ERR   0x82 //!< CRC mismatch
#define SERIAL_RC_UKWN_ERR  0x83 //!< Unknown command
#define SERIAL_RC_RANGE_ERR 0x84 //!< Incorrect range. TS will not retry command
#define SERIAL_RC_BUSY_ERR  0x85 //!< TS will wait and retry

#define SERIAL_TIMEOUT 3000

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

unsigned int makeWord(unsigned char h, unsigned char l);

const char serialVersion[] = { SERIAL_RC_OK, '0', '0', '2' };
const char canId[] = { SERIAL_RC_OK, 0 };
//const char codeVersion[] = { SERIAL_RC_OK, 'f','a','k','e','d','u','i','n','o',' ','2','0','2','3','0','6' }; //Note no null terminator in array and statu variable at the start
const char codeVersion[] = { SERIAL_RC_OK, 's','p','e','e','d','u','i','n','o',' ','2','0','2','3','0','5' }; //Note no null terminator in array and statu variable at the start
//const char productString[] = { SERIAL_RC_OK, 'F', 'a', 'k', 'e', 'd', 'u', 'i', 'n', 'o', ' ', '2', '0', '2', '3', '.', '0', '6' };
const char productString[] = { SERIAL_RC_OK, 'S', 'p', 'e', 'e', 'd', 'u', 'i', 'n', 'o', ' ', '2', '0', '2', '3', '.', '0', '5' };
const char testCommsResponse[] = { SERIAL_RC_OK, 255 };

#define UPDC32(octet, crc) (crc_32_tab[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8))

inline const CRC::Parameters<crcpp_uint32, 32>& CRC_32_JAVA()
{
	static const CRC::Parameters<crcpp_uint32, 32> parameters = { 0xEDB88320, 0xFFFFFFFF, 0xFFFFFFFF, true, true };
	return parameters;
}

class TSCpp
{
public:
	// Initiates TSC++
	TSCpp(std::string port, int baud);
	~TSCpp();

	TSCpp* GetInstance() { return instance; };

	// Waits for TSC++ to finish
	void WaitForThreads();

	// Tells the TSC++ thread to stop on the next loop
	void StopThreads();

	// Writes a byte to serial
	void Write(char value);
	// Writes a string to serial
	void Write(std::string value);
	// Writes a uint32 to serial
	uint32_t Write(uint32_t value);
	// Writes a uint16 to serial
	void Write(uint16_t value);
	
	// Reads a byte from serial
	int Read();

	// Reads a uint32 from serial
	uint32_t Read32();

	// Reverses the byte order of a uint32
	uint32_t ReverseBytes(uint32_t i);

	bool CheckTimeout();

	volatile struct statuses currentStatus;

private:
	static TSCpp* instance;
	serialib Serial;
	DLog Log;
	DEEPROM EEPROM;

	std::thread recvThread;
	std::thread timerThread;

	uint32_t loops = 0;
	bool quit = false;
	bool allowLegacy = true;

	int serialPayloadLength;
	int serialBytesRxTx;
	char serialPayload[264];
	uint64_t serialReceiveStartTime;

	void LegacyComms(char c);
	
	void Comms(char c);
	void SerialCommand();
	void SendReturnCode(char returnCode);
	
	void SendPayload(uint16_t size);
	void SendBufferAndCRC(uint16_t size);

	void FlushRXBuffer();

	uint64_t milis();
	void threadFn();
	void timerFn();

	// ~~Straight up ported from Speeduino:~~
	Pages pages;

	void loadPageValuesToBuffer(uint8_t pageNum, uint16_t offset, char* buffer, uint16_t length, int bufferOffset = 0);

	void generateLiveValues(uint16_t offset, uint16_t packetLength);
	byte getTSLogEntry(uint16_t byteNum);
	uint16_t freeRam();
};

void gentbl(void);
uint32_t crc32(uint8_t* bfr, size_t size);
