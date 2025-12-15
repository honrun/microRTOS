#include <stdint.h>
#include "DevicesTime.h"
#include "cpuusage.h"


/* us */
#define CPU_USAGE_CALC_TICK    100000LL

/* CPU 使用率 */
float g_fCPUUsage = 0.0f;


void vCPUUsageIdleHook(void)
{
    static uint32_t st_uiCountMax = 0;
    int64_t lTickStop;
    uint32_t uiTickNow = 0;

    for(lTickStop = lTimebaseGet() + CPU_USAGE_CALC_TICK; lTimebaseGet() < lTickStop; ++uiTickNow);

    st_uiCountMax = st_uiCountMax < uiTickNow ? uiTickNow : st_uiCountMax;

    /* get CPU usage */
    uiTickNow = st_uiCountMax - uiTickNow;
    g_fCPUUsage = (float)uiTickNow / (float)st_uiCountMax;
}

float fCPUUsageGet(void)
{
    return g_fCPUUsage;
}
