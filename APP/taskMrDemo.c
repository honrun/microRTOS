#include "stdint.h"
#include "stdio.h"
#include "cpuusage.h"
#include "DevicesWatchDog.h"
#include "microConfig.h"
#include "taskMrDemo.h"


/* 任务句柄 */
MrTaskType *g_ptypeMrTaskAddHandle;
MrTaskType *g_ptypeMrTaskDelHandle;

/* 定时器句柄 */
MrTimerType *g_ptypeMrTimerDemoHandle;

/* 消息队列句柄 */
MrMessageType *g_ptypeMrMessageHandle;


void vApplicationIdleHook(void)
{
    /* 喂狗 */
    vWatchdogReload();

    vCPUUsageIdleHook();
}

void vMrTaskAdd(size_t xParameters)
{
    while(1)
    {
        ++xParameters;

        cMrMessageSend(g_ptypeMrMessageHandle, &xParameters, 0);

        vMrDelayMs(NULL, 100);
    }
}

void vMrTaskDel(size_t xParameters)
{
    while(1)
    {
        cMrMessageRead(g_ptypeMrMessageHandle, &xParameters, 10);

        xParameters += xParameters;
    }
}


void vMrTimerDemo(size_t xParameters)
{
    xParameters += xParameters;
}


void vMrRtosInit(void)
{
    /* 创建消息队列 */
    cMrMessageCreate(&g_ptypeMrMessageHandle, MR_MESSAGE_TYPE_FIFO, sizeof(size_t), 8, 0);


    /* 创建任务 */
    cMrTaskCreate(&g_ptypeMrTaskAddHandle, vMrTaskAdd, "Add",   2, 256, 100000);
    cMrTaskCreate(NULL,                    vMrTaskAdd, "Add 2", 2, 256, 200000);

    cMrTaskCreate(&g_ptypeMrTaskDelHandle, vMrTaskDel, "Del",   4, 256, 300000);


    /* 创建定时器 */
    cMrTimerCreate(&g_ptypeMrTimerDemoHandle, vMrTimerDemo, (size_t)&g_ptypeMrTimerDemoHandle, 100);
    /* 启动定时器 */
    cMrTimerStart(g_ptypeMrTimerDemoHandle, MrTimerCirculation);


    /* 启动RTOS运行 */
    cMrTaskStart();
}
