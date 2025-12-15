#ifndef _DevicesFlash_H_
#define _DevicesFlash_H_


#define FLASH_PAGE_SIZE             ((uint32_t)2048)
#define FLASH_BASE_ADDR             ((uint32_t)0x08000000)
#define FLASH_USER_MAX_ADDR         (FLASH_BASE_ADDR + (512 * 1024))



int8_t cFlashWriteDatas(uint32_t uiAddress, const void *pvBuff, int32_t iLength);
int8_t cFlashReadDatas(uint32_t uiAddress, void *pvBuff, int32_t iLength);


#endif
