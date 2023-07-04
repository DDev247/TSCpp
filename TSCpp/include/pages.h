
#pragma once

#include <stdint.h>

struct Pages {

	// write all FF to all page files.
	void InitPages();

	/**
	 * Page count, as defined in the INI file
	 */
	uint8_t getPageCount(void);

	/**
	 * Page size in bytes
	 */
	uint16_t getPageSize(byte pageNum);

	uint32_t calculatePageCRC32(byte pageNum);

	uint8_t getPageValue(uint8_t pageNum, uint16_t offset);
	uint8_t* getPage(uint8_t pageNum);

	void setPageValues(uint8_t pageNum, uint16_t offset, char* buffer, uint16_t length, int buffOffset=0);
	void setPage(uint8_t pageNum, uint8_t* value);
};


// These are the page numbers that the Tuner Studio serial protocol uses to transverse the different map and config pages.
#define veMapPage     2
#define veSetPage     1 //Note that this and the veMapPage were swapped in Feb 2019 as the 'algorithm' field must be declared in the ini before it's used in the fuel table
#define ignMapPage    3
#define ignSetPage    4//Config Page 2
#define afrMapPage    5
#define afrSetPage    6//Config Page 3
#define boostvvtPage  7
#define seqFuelPage   8
#define canbusPage    9//Config Page 9
#define warmupPage    10 //Config Page 10
#define fuelMap2Page  11
#define wmiMapPage    12
#define progOutsPage  13
#define ignMap2Page   14
#define boostvvtPage2 15
