#include "stdint.h"
#include "DevicesTime.h"
#include "DevicesRandom.h"


static uint32_t st_uiRandSeed = 123;


void vSrandSoft(uint32_t uiRandSeed)
{
    st_uiRandSeed = uiRandSeed;
}

uint32_t uiRandSoft(void)
{
    const uint32_t uiMultiplier = 1103515245, uiIncrement = 12345;
    static int8_t cCnt = 0;

    /* 增加混乱程度 */
    if(((cCnt++) % 17) == 0)
    {
        vSrandSoft(lTimebaseGet());
    }

    /* 线性同余 */
    st_uiRandSeed = st_uiRandSeed * uiMultiplier + uiIncrement;

    return st_uiRandSeed;
}
