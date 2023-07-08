
#include <eeprom.h>

DEEPROM::DEEPROM() {
    //eepromFile = std::fstream("tscpp/eeprom.bin", std::ios::binary | std::ios::in | std::ios::out);
}

DEEPROM::~DEEPROM() {
    eepromFile.flush();
    eepromFile.close();
}

void DEEPROM::write(int address, uint8_t value) {
    eepromFile = std::fstream("tscpp/eeprom.bin", std::ios::binary | std::ios::out);
    if (!eepromFile) {
        std::cout << "Failed to open EEPROM file..." << std::endl;
        return;
    }

    eepromFile.seekp(address);
    eepromFile.put(value);
    eepromFile.flush();
    eepromFile.close();
}

uint8_t DEEPROM::read(int address) {
    eepromFile = std::fstream("tscpp/eeprom.bin", std::ios::binary | std::ios::in);
    if (!eepromFile) {
        std::cout << "Failed to open EEPROM file..." << std::endl;
        return 0;
    }
    eepromFile.seekg(address);
    uint8_t result = eepromFile.get();
    eepromFile.close();
    return result;
}