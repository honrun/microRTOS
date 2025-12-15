/*
 * File:   UserDelay.c
 * Author: Honrun
 */

#include "gd32f30x.h"
#include "stdint.h"
#include "DevicesTime.h"
#include "DevicesDelay.h"

/*
 * Return:      void
 * Parameters:  void
 * Description: DWT初始化
 */
void vDWTDelayInit(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/*
 * Return:      void
 * Parameters:  Time: 延时时间
 * Description: 秒延时
 */
void vDWTDelayS(float fTime)
{
    vDWTDelayUs(fTime * 1000000.0f);
}

/*
 * Return:      void
 * Parameters:  Time: 延时时间
 * Description: 毫秒延时
 */
void vDWTDelayMs(float fTime)
{
    vDWTDelayUs(fTime * 1000.0f);
}

/*
 * Return:      void
 * Parameters:  Time: 延时时间
 * Description: 微秒延时
 */
void vDWTDelayUs(float fTime)
{
    uint32_t uiTimeStart = DWT->CYCCNT, uiTimeStop;

    if(fTime < 0.1f)
        return;

    uiTimeStop  = (uint32_t)((SystemCoreClock / 1000000u) * fTime) + uiTimeStart;

    if(uiTimeStop >= uiTimeStart)
        while((DWT->CYCCNT > uiTimeStart) & (DWT->CYCCNT < uiTimeStop));
    else
        while(!((DWT->CYCCNT > uiTimeStop) && (DWT->CYCCNT < uiTimeStart)));
}

/*
 * Return:      void
 * Parameters:  void
 * Description: DWT初始化
 */
void vTimerDelayInit(void)
{
    // cTimer5Init();
}

/*
 * Return:      void
 * Parameters:  Time: 延时时间
 * Description: 秒延时
 */
void vTimerDelayS(float fTime)
{
    vTimerDelayUs(fTime * 1000000.0f);
}

/*
 * Return:      void
 * Parameters:  Time: 延时时间
 * Description: 毫秒延时
 */
void vTimerDelayMs(float fTime)
{
    vTimerDelayUs(fTime * 1000.0f);
}

/*
 * Return:      void
 * Parameters:  Time: 延时时间
 * Description: 微秒延时
 */
void vTimerDelayUs(float fTime)
{
    int64_t lTimeStop = lTimebaseGet() + (int64_t)fTime;

    if(fTime < 1.0f)
        return;

    while(lTimebaseGet() < lTimeStop);
}
