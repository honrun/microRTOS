#include "stdio.h"
#include "stdint.h"
#include "microConfig.h"
#include "microTimer.h"


/* 定时器链表 */
MrListType g_typeMrListTimer = {&g_typeMrListTimer, &g_typeMrListTimer};


int8_t cMrTimerCreate(MrTimerType **pptypeTimer, vMrTimerFunction pvFunction, size_t xParameters, int64_t lTick)
{
    MrTimerType *ptypeTimer;

    if((pptypeTimer == NULL) || (pvFunction == NULL))
        return 1;

    /* 申请定时器控制块 */
    if((ptypeTimer = pvMrMemoryCalloc(1, sizeof(MrTimerType))) == NULL)
        return 2;

    cMrListInit(&ptypeTimer->list, 0);
    ptypeTimer->list.pElement = ptypeTimer;
    ptypeTimer->pFunction     = pvFunction;
    ptypeTimer->parameters    = xParameters;
    ptypeTimer->duration      = lTick;

    *pptypeTimer = ptypeTimer;

    return 0;
}

/* 启动定时器 */
int8_t cMrTimerStart(MrTimerType *ptypeTimer, MrTimerStateEnum enumState)
{
    if(ptypeTimer == NULL)
        return 1;

    /* 进入临界段 */
    vMrDisableIsr();

    /* 从当前链表中移除 */
    cMrListRemove(&ptypeTimer->list);

    ptypeTimer->state       = enumState;
    ptypeTimer->list.value  = lMrTickGet();
    ptypeTimer->list.value += (ptypeTimer->state & MrTimerImmediately) ? 0 : ptypeTimer->duration;

    /* 插入到定时器链表 */
    cMrListInsert(&g_typeMrListTimer, &ptypeTimer->list);

    /* 退出临界段 */
    vMrEnableIsr();

    /* 触发定时器任务执行一次 */
    cMrTaskResume(g_ptypeMrTaskTimer);

    return 0;
}

/* 停止定时器 */
int8_t cMrTimerStop(MrTimerType *ptypeTimer)
{
    if(ptypeTimer == NULL)
        return 1;

    /* 进入临界段 */
    vMrDisableIsr();

    /* 从当前链表中移除 */
    cMrListRemove(&ptypeTimer->list);

    ptypeTimer->state = MrTimerStop;

    /* 退出临界段 */
    vMrEnableIsr();

    return 0;
}

/* 定时器溢出检查任务 */
void vMrTaskTimer(size_t xParameters)
{
    MrListType *ptypeList = NULL;
    MrTimerType *ptypeTimer = NULL;

    while(1)
    {
        /* 把遍历指针指向就绪链表首部 */
        while((ptypeList = g_typeMrListTimer.pNext) != &g_typeMrListTimer)
        {
            /* 排序存储，第一个不满足，后面的也不会满足 */
            if(lMrTickGet() < ptypeList->value)
                break;

            /* 进入临界段 */
            vMrDisableIsr();

            /* 从定时器链表中移除 */
            cMrListRemove(ptypeList);

            /* 退出临界段 */
            vMrEnableIsr();

            ptypeTimer = (MrTimerType *)ptypeList->pElement;

            /* 执行回调函数 */
            ptypeTimer->pFunction(ptypeTimer->parameters);

            /* 循环计时 */
            if(ptypeTimer->state & MrTimerCirculation)
            {
                ptypeList->value += ptypeTimer->duration;

                /* 进入临界段 */
                vMrDisableIsr();

                /* 重新插入到定时器链表 */
                cMrListInsert(&g_typeMrListTimer, ptypeList);

                /* 退出临界段 */
                vMrEnableIsr();
            }
        }

        /* 下次定时器溢出还需要的 tick 数量 */
        vMrDelayTick(NULL, g_typeMrListTimer.pNext->value - lMrTickGet());
    }
}
