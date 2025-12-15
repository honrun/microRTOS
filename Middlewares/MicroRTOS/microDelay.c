/*
 *  modifier: Honrun
 *  date:     2023/10/29 20:42
 */
#include "stdio.h"
#include "microConfig.h"
#include "microDelay.h"


/* 延时链表 */
MrListType g_typeMrListDelay = {&g_typeMrListDelay, &g_typeMrListDelay};


/* 延时 n Tick */
void vMrDelayTick(int64_t *plTickLast, int64_t lTick)
{
    MrTaskType *ptypeTask = (MrTaskType *)g_ptypeMrTaskCurrent;

    /* 进入临界段 */
    vMrDisableIsr();

    /* 计算延时 Tick 数量 */
    if(plTickLast != NULL)
    {
        *plTickLast += lTick;
        ptypeTask->listDelay.value = *plTickLast;
    }
    else
    {
        ptypeTask->listDelay.value = lMrTickGet() + lTick;
    }

    /* 从链表中移除 */
    cMrListRemove(&ptypeTask->listReady);

    /* 插入到延时链表 */
    cMrListInsert(&g_typeMrListDelay, &ptypeTask->listDelay);

    /* 退出临界段 */
    vMrEnableIsr();

    /* 触发任务切换 */
    vMrCpuYield();
}

/* 延时 n 毫秒 */
void vMrDelayMs(int64_t *plTickLast, int64_t lTime)
{
    lTime /= (int64_t)(1000 / configMr_RTOS_CLOCK_HZ);

    vMrDelayTick(plTickLast, lTime);
}

/* 判断延时事件是否溢出 */
void vMrDelayCheck(void)
{
    MrListType *ptypeList;
    MrTaskType *ptypeTask;
    int64_t lTime = lMrTickGet();

    /* 进入临界段 */
    vMrDisableIsr();

    /* 把遍历指针指向就绪链表首部 */
    while((ptypeList = g_typeMrListDelay.pNext) != &g_typeMrListDelay)
    {
        /* 排序存储，第一个不满足，后面的也不会满足 */
        if(lTime < ptypeList->value)
            break;

        ptypeTask = (MrTaskType *)ptypeList->pElement;

        /* 从链表中移除 */
        cMrListRemove(&ptypeTask->listEvent);
        cMrListRemove(&ptypeTask->listDelay);

        /* 插入到就绪链表 */
        ptypeTask->listReady.value = ptypeTask->priority;
        cMrListInsert(&g_typeMrListTaskReady, &ptypeTask->listReady);
    }

    /* 退出临界段 */
    vMrEnableIsr();
}
