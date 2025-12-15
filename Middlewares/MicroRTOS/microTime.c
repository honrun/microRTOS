/*
 *  modifier: Honrun
 *  date:     2023/10/29 20:42
 */
#include "stdint.h"
#include "microTime.h"


/* 系统 Tick */
int64_t g_lMrTick = 0;


/* 获取系统 Tick */
int64_t lMrTickGet(void)
{
    return g_lMrTick;
}

/* 系统 Tick 周期递增 */
void vMrTickGather(void)
{
    ++g_lMrTick;
}

/* 系统启动后，到此刻的运行时间（us） */
int64_t lMrTimeMonotonicGet(void)
{
    extern int64_t lTimebaseGet(void);
    return lTimebaseGet();
}
