
#include <eeprom.h>

DEEPROM::DEEPROM() {
	eepromFile = std::fstream("eeprom.bin", std::ios::binary | std::ios::in | std::ios::out);
}

DEEPROM::~DEEPROM() {
	eepromFile.flush();
	eepromFile.close();
}

void DEEPROM::write(int address, uint8_t value) {
    eepromFile.seekp(address);
    eepromFile.put(value);
}

uint8_t DEEPROM::read(int address) {
    eepromFile.seekg(address);
    return eepromFile.get();
}