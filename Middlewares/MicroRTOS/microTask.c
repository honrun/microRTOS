/*
 *  modifier: Honrun
 *  date:     2023/10/29 20:42
 */
#include "stdio.h"
#include "string.h"
#include "microConfig.h"
#include "microTask.h"


/* 当前正在运行的任务 */
volatile MrTaskType *g_ptypeMrTaskCurrent = NULL;

/* 空闲任务控制块 */
MrTaskType *g_ptypeMrTaskIdle = NULL;

/* 定时器任务控制块 */
MrTaskType *g_ptypeMrTaskTimer = NULL;

/* 就绪任务链表 */
MrListType g_typeMrListTaskReady = {&g_typeMrListTaskReady, &g_typeMrListTaskReady};


void vMrTaskIdle(uint32_t uiParameters)
{
    while(1)
    {
        /* 触发任务切换 */
        vMrCpuYield();

        /* 空闲任务钩子函数 */
        extern void vApplicationIdleHook(void);
        vApplicationIdleHook();
    }
}

void vMrTaskError(void)
{
    while(1)
    {
    }
}

int8_t cMrTaskCreate(MrTaskType **pptypeTask, vMrTaskFunction pvFunction, char *pcName, int32_t iPriority, int32_t iStackSize, uint32_t uiParameters)
{
    MrTaskType *ptypeTask;
    uint8_t *pucStack;

    if(pvFunction == NULL)
        return 1;

    /* 申请任务控制块 */
    if((ptypeTask = pvMrMemoryCalloc(1, sizeof(MrTaskType))) == NULL)
        return 2;

    iStackSize = (iStackSize < configMr_TASK_STACK_MIN_SIZE) ? configMr_TASK_STACK_MIN_SIZE : iStackSize;

    /* N 字节对齐 */
    iStackSize = mrMemoryRoundUp(iStackSize);

    /* 申请任务栈 */
    if((pucStack = pvMrMemoryCalloc(iStackSize, sizeof(uint8_t))) == NULL)
    {
        vMrMemoryFree(ptypeTask);
        return 3;
    }

    /* 任务栈初始化 */
    ptypeTask->pStack = puiMrTaskStackInit((uint32_t *)(pucStack + iStackSize), pvFunction, uiParameters);

    /* 任务名 */
    ptypeTask->name = pcName;

    /* 任务优先级 */
    ptypeTask->priority = iPriority;

    /* 初始化链表信息 */
    cMrListInit(&ptypeTask->listReady, iPriority);
    cMrListInit(&ptypeTask->listEvent, 0);
    cMrListInit(&ptypeTask->listDelay, 0);
    ptypeTask->listReady.pElement = ptypeTask->listEvent.pElement = ptypeTask->listDelay.pElement = ptypeTask;

    /* 进入临界段 */
    vMrDisableIsr();

    /* 插入到就绪链表 */
    cMrListInsert(&g_typeMrListTaskReady, &ptypeTask->listReady);

    /* 退出临界段 */
    vMrEnableIsr();

    if(pptypeTask != NULL)
        *pptypeTask = ptypeTask;

    return 0;
}

/* 恢复任务 */
int8_t cMrTaskResume(MrTaskType *ptypeTask)
{
    if(ptypeTask == NULL)
        return 1;

    /* 进入临界段 */
    vMrDisableIsr();

    /* 从当前链表中移除 */
    cMrListRemove(&ptypeTask->listReady);
    cMrListRemove(&ptypeTask->listEvent);
    cMrListRemove(&ptypeTask->listDelay);

    /* 插入到就绪链表 */
    ptypeTask->listReady.value = ptypeTask->priority;
    cMrListInsert(&g_typeMrListTaskReady, &ptypeTask->listReady);

    /* 退出临界段 */
    vMrEnableIsr();

    /* 触发任务切换 */
    vMrCpuYield();

    return 0;
}

/* 挂起任务 */
int8_t cMrTaskSuspend(MrTaskType *ptypeTask)
{
    ptypeTask = (ptypeTask == NULL) ? (MrTaskType *)g_ptypeMrTaskCurrent : ptypeTask;

    /* 进入临界段 */
    vMrDisableIsr();

    /* 从链表中移除 */
    cMrListRemove(&ptypeTask->listReady);
    cMrListRemove(&ptypeTask->listEvent);
    cMrListRemove(&ptypeTask->listDelay);

    /* 退出临界段 */
    vMrEnableIsr();

    /* 触发任务切换 */
    vMrCpuYield();

    return 0;
}

/* 查找当前就绪链表内最高优先级的任务 */
MrTaskType *ptypeMrTaskReadyHighGet( void )
{
    /* 从小到大，排序存储，最后一个就是优先级最高的任务 */
    return (MrTaskType *)g_typeMrListTaskReady.pPrev->pElement;
}

/* 更新到最高优先级 */
void vMrTaskSwitchContext( void )
{
    /* 进入临界段 */
    vMrDisableIsr();

    g_ptypeMrTaskCurrent = ptypeMrTaskReadyHighGet();

    /* 退出临界段 */
    vMrEnableIsr();
}

int8_t cMrTaskStart(void)
{
    /* 创建空闲任务 */
    cMrTaskCreate(&g_ptypeMrTaskIdle,  vMrTaskIdle,  "IDLE", -999, configMr_TASK_STACK_MIN_SIZE,  0);

    /* 创建定时器任务 */
    cMrTaskCreate(&g_ptypeMrTaskTimer, vMrTaskTimer, "Timer", configMr_TASK_PRIORITY_TIMER, configMr_TASK_STACK_TIMER_SIZE, 0);

    /* 系统启动时，先指向空闲任务 */
    g_ptypeMrTaskCurrent = g_ptypeMrTaskIdle;

    /* 系统时钟初始化 */
    cMrCpuTickInit();

    /* 启动RTOS调度 */
    vMrCpuStart();

    /* RTOS正常启动后，不会运行到这 */
    vMrTaskError();

    return 1;
}
