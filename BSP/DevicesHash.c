#include "stdint.h"
#include "DevicesHash.h"


uint32_t uiSDBMHash(void *pvDatas, int32_t iLength)
{
    uint8_t *pcData = pvDatas;
    uint32_t hash = 0;

    while((iLength--) > 0)
    {
        hash = (hash << 16) + (hash << 6) - hash + *pcData++;
    }

    return (hash & 0x7FFFFFFF);
}

uint32_t uiRSHash(void *pvDatas, int32_t iLength)
{
    uint8_t *pcData = pvDatas;
    uint32_t hash = 0, a = 63689;

    while((iLength--) > 0)
    {
        hash = hash * a + *pcData++;
        a *= 378551;
    }

    return (hash & 0x7FFFFFFF);
}

uint32_t uiJSHash(void *pvDatas, int32_t iLength)
{
    uint8_t *pcData = pvDatas;
    uint32_t hash = 1315423911;

    while((iLength--) > 0)
    {
        hash ^= (hash << 5) + (hash >> 2) + *pcData++;
    }

    return (hash & 0x7FFFFFFF);
}

uint32_t uiPJWHash(void *pvDatas, int32_t iLength)
{
    uint8_t *pcData = pvDatas;
    uint32_t HighBits = 0xFFFFFFFFUL << (sizeof(uint32_t) * 8 - sizeof(uint32_t));
    uint32_t hash = 0, test = 0;

    while((iLength--) > 0)
    {
        hash = (hash << sizeof(uint32_t)) + *pcData++;
        if ((test = hash & HighBits) != 0)
        {
            hash = (hash ^ (test >> ((sizeof(uint32_t) * 8 * 3) / 4))) & ~HighBits;
        }
    }

    return (hash & 0x7FFFFFFF);
}

uint32_t uiELFHash(void *pvDatas, int32_t iLength)
{
    uint8_t *pcData = pvDatas;
    uint32_t hash = 0, x = 0;

    while((iLength--) > 0)
    {
        hash = (hash << 4) + *pcData++;
        if ((x = hash & 0xF0000000UL) != 0)
        {
            hash = (hash ^ (x >> 24)) & ~x;
        }
    }

    return (hash & 0x7FFFFFFF);
}

uint32_t uiBKDRHash(void *pvDatas, int32_t iLength)
{
    uint8_t *pcData = pvDatas;
    uint32_t hash = 0;

    while((iLength--) > 0)
    {
        // 31 131 1313 13131 131313 etc..
        hash = hash * 131 + *pcData++;
    }

    return (hash & 0x7FFFFFFF);
}

uint32_t uiDJBHash(void *pvDatas, int32_t iLength)
{
    uint8_t *pcData = pvDatas;
    uint32_t hash = 5381;

    while((iLength--) > 0)
    {
        hash += (hash << 5) + *pcData++;
    }

    return (hash & 0x7FFFFFFF);
}

uint32_t uiAPHash(void *pvDatas, int32_t iLength)
{
    uint8_t *pcData = pvDatas;
    uint32_t hash = 0, i = 0;

    for (i = 0; i < iLength; ++i)
    {
        hash ^= (i & 1) ? ~((hash << 11) ^ (*pcData++) ^ (hash >> 5)) : (hash << 7) ^ (*pcData++) ^ (hash >> 3);
    }

    return (hash & 0x7FFFFFFF);
}
