#ifndef _DevicesHash_H_
#define _DevicesHash_H_


uint32_t uiSDBMHash(void *pvDatas, int32_t iLength);
uint32_t uiRSHash(void *pvDatas, int32_t iLength);
uint32_t uiJSHash(void *pvDatas, int32_t iLength);
uint32_t uiPJWHash(void *pvDatas, int32_t iLength);
uint32_t uiELFHash(void *pvDatas, int32_t iLength);
uint32_t uiBKDRHash(void *pvDatas, int32_t iLength);
uint32_t uiDJBHash(void *pvDatas, int32_t iLength);
uint32_t uiAPHash(void *pvDatas, int32_t iLength);


#endif
