#include "stdint.h"
#include "DevicesUID.h"


char *pucMcuUIDRead(void)
{
    static char ucUIDBuff[25] = {0};
    uint8_t *pIDAddress, i;

    if(ucUIDBuff[0] == 0)
    {
        for(pIDAddress = (uint8_t *)CHIP_UID_BASE, i = 0; i < 12; ++i, ++pIDAddress)
        {
            ucUIDBuff[i * 2]     = "0123456789ABCDEF"[*pIDAddress >> 4];
            ucUIDBuff[i * 2 + 1] = "0123456789ABCDEF"[*pIDAddress & 0xF];
        }
    }

    return ucUIDBuff;
}
