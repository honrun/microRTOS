#ifndef _DEVICES_DELAY_H_
#define _DEVICES_DELAY_H_


/* 以下两类延时函数可以调用 */

/* 阻塞似延时 */
#define vDelayInit  vTimerDelayInit
#define vDelayS     vTimerDelayS
#define vDelayMs    vTimerDelayMs
#define vDelayUs    vTimerDelayUs

/* 非阻塞似延时 */
void vRtosDelayS(float fTime);
void vRtosDelayMs(float fTime);





/* 请勿调用以下函数 */
void vDWTDelayInit(void);
void vDWTDelayS(float fTime);
void vDWTDelayMs(float fTime);
void vDWTDelayUs(float fTime);

void vTimerDelayInit(void);
void vTimerDelayS(float fTime);
void vTimerDelayMs(float fTime);
void vTimerDelayUs(float fTime);


#endif
