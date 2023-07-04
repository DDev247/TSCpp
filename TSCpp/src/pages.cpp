
#include <tscpp.h>
#include <pages.h>
#include <storage.h>
#include <string>

constexpr const uint16_t ini_page_sizes[] = { 0, 128, 288, 288, 128, 288, 128, 240, 384, 192, 192, 288, 192, 128, 288, 256 };

void Pages::InitPages() {
    printf("Initiating Pages...\n");
    uint8_t count = getPageCount();

    for (int i = 0; i < count; i++) {
        uint16_t size = getPageSize(i);
        std::fstream file = std::fstream("page" + std::to_string(i) + ".bin", std::ios::binary | std::ios::out);
        if (!file) {
            std::cerr << "Failed to open Page file." << std::endl;
        }

        // write n 0xFF's
        for (int i = 0; i < size; i++) {
            file.put(0xFF);
        }
        
        file.flush();
        file.close();
    }
    
    printf("Initiated %i pages.\n", count);
}

uint8_t Pages::getPageCount(void) {
    return _countof(ini_page_sizes);
}

uint16_t Pages::getPageSize(byte pageNum) {
    return ini_page_sizes[pageNum];
}

uint32_t Pages::calculatePageCRC32(byte pageNum) {
    gentbl();

    uint8_t* page = getPage(pageNum);
    uint16_t size = getPageSize(pageNum);

    //return crc32(page, size);
    return CRC::Calculate(page, size, CRC::CRC_32());
}

uint8_t Pages::getPageValue(uint8_t pageNum, uint16_t offset) {
    std::fstream file = std::fstream("page" + std::to_string(pageNum) + ".bin", std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        std::cerr << "Failed to open Page file." << std::endl;
        return 0;
    }
    file.seekg(offset);
    uint8_t value = file.get();
    file.close();
    return value;
}

uint8_t* Pages::getPage(uint8_t pageNum) {
    std::fstream file = std::fstream("page" + std::to_string(pageNum) + ".bin", std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        std::cerr << "Failed to open Page file." << std::endl;
        return 0;
    }
    const uint16_t size = ini_page_sizes[pageNum];
    uint8_t* buf = new uint8_t[size];
    file.read((char*)buf, size);
    file.close();
    return buf;
}

void Pages::setPageValues(uint8_t pageNum, uint16_t offset, char* buffer, uint16_t length, int buffOffset) {
    std::fstream file = std::fstream("page" + std::to_string(pageNum) + ".bin", std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        std::cerr << "Failed to open Page file." << std::endl;
        return;
    }
    file.seekp(offset);
    char* buf = new char[sizeof(buffer) - buffOffset];
    memcpy(buf, buffer + buffOffset, sizeof(buffer) - buffOffset);

    file.write(buf, length);
    file.flush();

    delete[] buf;
    file.close();
}

void Pages::setPage(uint8_t pageNum, uint8_t* buf) {
    std::fstream file = std::fstream("page" + std::to_string(pageNum) + ".bin", std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        std::cerr << "Failed to open Page file." << std::endl;
        return;
    }
    const uint16_t size = ini_page_sizes[pageNum];
    file.write((char*)buf, size);
    file.flush();
    file.close();
}
