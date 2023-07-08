
#include <eeprom.h>
#include <storage.h>

DEEPROM::DEEPROM() {
    std::fstream file = std::fstream("tscpp/eeprom.bin", std::ios::binary | std::ios::in);
    if (!file) {
        std::cout << "Failed to open EEPROM file..." << std::endl;
        return;
    }
    std::cout << "EEPROM LOAD" << std::endl;
    file.read((char*)eeprom, eepromCount);
}

DEEPROM::~DEEPROM() {
    //delete[] eeprom;
}

void DEEPROM::writeFile() {
    std::fstream file = std::fstream("tscpp/eeprom.bin", std::ios::binary | std::ios::out);
    if (!file) {
        std::cout << "Failed to open EEPROM file..." << std::endl;
        return;
    }
    std::cout << "EEPROM DUMP" << std::endl;
    file.write((const char*)eeprom, eepromCount);
}

void DEEPROM::write(int address, uint8_t value) {
    /*std::fstream file = std::fstream("tscpp/eeprom.bin", std::ios::binary | std::ios::out);
    if (!file) {
        std::cout << "Failed to open EEPROM file..." << std::endl;
        return;
    }*/

    std::cout << "EEPROM << " << value << " @ " << address << std::endl;

    eeprom[address] = value;

    //file.seekp(address);
    //file.put(value);
    //file.flush();
    //file.close();
}

uint8_t DEEPROM::read(int address) {
    /*std::fstream file = std::fstream("tscpp/eeprom.bin", std::ios::binary | std::ios::in);
    if (!file) {
        std::cout << "Failed to open EEPROM file..." << std::endl;
        return 0;
    }
    file.seekg(address);
    uint8_t result = file.get();
    file.close();*/

    uint8_t result = eeprom[address];
    std::cout << "EEPROM >> " << result << " @ " << address << std::endl;
    return result;
}