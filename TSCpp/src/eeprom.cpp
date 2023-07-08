
#include <eeprom.h>
#include <storage.h>

DEEPROM::DEEPROM() {
    //eepromFile = std::fstream("tscpp/eeprom.bin", std::ios::binary | std::ios::in | std::ios::out);
    loadConfig();
}

DEEPROM::~DEEPROM() {
}

void DEEPROM::write(int address, uint8_t value) {
    std::ofstream file = std::ofstream("tscpp/eeprom.bin", std::ios::binary);
    if (!eepromFile) {
        std::cout << "Failed to open EEPROM file..." << std::endl;
        return;
    }

    file.seekp(address);
    file.put(value);
    file.flush();
    file.close();
}

uint8_t DEEPROM::read(int address) {
    std::ifstream file = std::ifstream("tscpp/eeprom.bin", std::ios::binary);
    if (!file) {
        std::cout << "Failed to open EEPROM file..." << std::endl;
        return 0;
    }
    file.seekg(address);
    uint8_t result = file.get();
    file.close();
    return result;
}