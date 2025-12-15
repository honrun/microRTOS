#include "gd32f30x.h"
#include "stdio.h"
#include "stdint.h"
#include "DevicesFlash.h"


/* 写入长度需要是 2的整数倍 */
int8_t cFlashWriteDatas(uint32_t uiAddress, const void *pvBuff, int32_t iLength)
{
    uint16_t *pusDataAddress = (uint16_t *)pvBuff;
    int8_t cError = 0, cCnt = 0;

    if((iLength < 1) || ((uiAddress + iLength) > FLASH_USER_MAX_ADDR))
    {
        printf("cFlashWriteDatas uiAddress: %08X iLength: %d >= FLASH_USER_MAX_ADDR.\r\n", uiAddress, iLength);
        return 1;
    }

    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

    /* 每次写入2字节 */
    while(iLength > 0)
    {
        /* erase the flash pages */
        if((uiAddress % FLASH_PAGE_SIZE) == 0)
        {
            cCnt = 8;
            /* erase flash */
            while((fmc_page_erase(uiAddress) != FMC_READY) & (--cCnt));

            /* clear all pending flags */
            fmc_flag_clear(FMC_FLAG_BANK0_END);
            fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
            fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

            if(cCnt == 0)
            {
                printf("cFlashWriteDatas fmc_page_erase addr: 0x%08X error.\r\n", (unsigned int)uiAddress);
                cError |= 2;
                break;
            }
        }

        /* program flash */
        if((*(volatile uint16_t *)uiAddress != *pusDataAddress) && (fmc_halfword_program(uiAddress, *pusDataAddress) != FMC_READY))
        {
            printf("cFlashWriteDatas HAL_FLASH_Program addr: 0x%08X error!\r\n", (unsigned int)uiAddress);
            cError |= 4;
            break;
        }

        /* clear all pending flags */
        fmc_flag_clear(FMC_FLAG_BANK0_END);
        fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
        fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

        ++pusDataAddress;
        uiAddress += 2;
        iLength -= 2;
    }

    /* lock the main FMC after the program operation */
    fmc_lock();

    return cError;
}

int8_t cFlashReadDatas(uint32_t uiAddress, void *pvBuff, int32_t iLength)
{
    uint8_t *pucBuff = pvBuff;

    if((iLength < 1) || ((uiAddress + iLength) > FLASH_USER_MAX_ADDR))
    {
        printf("cFlashReadDatas uiAddress: %08X iLength: %d >= FLASH_USER_MAX_ADDR.\r\n", uiAddress, iLength);
        return 1;
    }

    /* Read Flash */
    while((iLength--) > 0)
    {
        *pucBuff++ = *(volatile uint8_t*)uiAddress++;
    }

    return 0;
}
