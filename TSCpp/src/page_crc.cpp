#include "../include/globals.h"
#include "../include/page_crc.h"
#include "../include/pages.h"
#include "../include/table3d_axis_io.h"
#include <assert.h>

uint32_t page_crctbl[256];

void page_gentbl(void)
{
    uint32_t crc;
    uint32_t c;
    uint32_t i;
    for (c = 0; c < 0x100; c++) {
        crc = c;
        for (i = 0; i < 8; i++) {
            crc = (crc & 1) ? (crc >> 1) ^ 0xedb88320 : (crc >> 1);
        }
        page_crctbl[c] = crc;
    }
}

uint32_t page_crc32(uint8_t* bfr, size_t size)
{
    uint32_t crc = 0xfffffffful;
    while (size--)
        crc = (crc >> 8) ^ page_crctbl[(crc & 0xff) ^ *bfr++];
    return(crc ^ 0xfffffffful);
}

static inline uint32_t compute_raw_crc(const page_iterator_t &entity, bool update, uint32_t crc)
{
    if (update)
        return page_crc32((uint8_t*)entity.pData, entity.size);
    //return CRC::Calculate((uint8_t*)entity.pData, entity.size, CRC::CRC_32(), crc);
    
    page_gentbl();
    return page_crc32((uint8_t*)entity.pData, entity.size);
}

static inline uint32_t compute_row_crc(const table_row_iterator &row, bool update, uint32_t crc)
{
    if(update)
        return page_crc32((uint8_t*)*row, row.size());
    
    page_gentbl();
    return page_crc32((uint8_t*)*row, row.size());
}

static inline uint32_t compute_tablevalues_crc(table_value_iterator it)
{
    uint32_t crc = compute_row_crc(*it, false, 0);
    ++it;

    while (!it.at_end())
    {
        crc = compute_row_crc(*it, true, crc);
        ++it;
    }
    return crc;
}

static inline uint32_t compute_tableaxis_crc(table_axis_iterator it, uint32_t crc)
{
    const int16_byte *pConverter = table3d_axis_io::get_converter(it.get_domain());

    byte values[32]; // Fingers crossed we don't have a table bigger than 32x32
    byte *pValue = values;
    while (!it.at_end())
    {
        *pValue++ = pConverter->to_byte(*it);
        ++it;
    }
    return pValue-values==0 ? crc : page_crc32(values, pValue-values);
}

static inline uint32_t compute_table_crc(const page_iterator_t &entity, bool update)
{
    return compute_tableaxis_crc(y_begin(entity), 
                compute_tableaxis_crc(x_begin(entity),
                    compute_tablevalues_crc(rows_begin(entity))
                ));
}

static inline uint32_t pad_crc(uint16_t padding, uint32_t crc)
{
    uint8_t raw_value = 0u;
    while (padding>0)
    {
        //crc = CRC::Calculate(&raw_value, 1, CRC::CRC_32(), crc);
        crc = page_crc32(&raw_value, 1);
        --padding;
    }
    return crc;
}

static inline uint32_t compute_crc(const page_iterator_t &entity, bool update, uint32_t crc)
{
    switch (entity.type)
    {
    case Raw:
        return compute_raw_crc(entity, update, crc);
        break;

    case Table:
        return compute_table_crc(entity, update);
        break;

    case NoEntity:
        return pad_crc(entity.size, 0U);
        break;

    default:
        assert(0);
        break;
    }
}

uint32_t calculatePageCRC32(byte pageNum)
{
    page_iterator_t entity = page_begin(pageNum);
    // Initial CRC calc
    uint32_t crc = compute_crc(entity, false, 0);

    entity = advance(entity);
    while (entity.type!=End)
    {
        crc = compute_crc(entity, true, crc);
        entity = advance(entity);
    }
    return ~pad_crc(getPageSize(pageNum) - entity.size, crc);
}