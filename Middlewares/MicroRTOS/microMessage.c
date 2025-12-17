#include "stdio.h"
#include "string.h"
#include "microConfig.h"
#include "microMessage.h"


/* 创建消息控制块 */
int8_t cMrMessageCreate(MrMessageType **pptypeMessage, uint16_t usType, int16_t sSize, int16_t sLength, int16_t sLengthInit)
{
    MrMessageType *ptypeMessage;

    if((pptypeMessage == NULL) || (sSize < 1) || (sLength < 1))
        return 1;

    /* 申请消息控制块 */
    if((ptypeMessage = pvMrMemoryCalloc(1, sizeof(MrMessageType))) == NULL)
        return 2;

    /* 申请消息存储缓存 */
    if((ptypeMessage->head = pvMrMemoryMalloc((size_t)sSize * sLength)) == NULL)
    {
        vMrMemoryFree(ptypeMessage);
        return 3;
    }

    /* 初始化消息队列节点 */
    cMrListInit(&ptypeMessage->listSend, 0);
    cMrListInit(&ptypeMessage->listRead, 0);

    /* 互斥信号量 */
    ptypeMessage->taskMutex = NULL;
    ptypeMessage->priorityMutex = -1;

    /* 初始化消息队列类型 */
    ptypeMessage->type = usType;
    ptypeMessage->size = sSize;

    /* 初始化消息队列参数 */
    ptypeMessage->tail  = ptypeMessage->head + (size_t)sSize * sLength;
    ptypeMessage->index = ptypeMessage->head + (size_t)sSize * sLengthInit;
    ptypeMessage->send  = ptypeMessage->read = ptypeMessage->head;

    *pptypeMessage = ptypeMessage;

    return 0;
}

/* 获取消息 */
int8_t cMrMessageRead(MrMessageType *ptypeMessage, void *pvData, int64_t lTick)
{
    MrTaskType *ptypeTask = (MrTaskType *)g_ptypeMrTaskCurrent;
    int8_t cError = 2;

    if(ptypeMessage == NULL)
        return 1;

    /* 进入临界段 */
    vMrDisableIsr();

    /* 消息队列中已没有消息 */
    if((ptypeMessage->index <= ptypeMessage->head) && (lTick != 0LL))
    {
        /* 互斥类型 */
        if(ptypeMessage->type == MR_MESSAGE_TYPE_MUTUAL)
        {
            /* 提高优先级 */
            if(ptypeMessage->taskMutex->priority < ptypeTask->priority)
            {
                ptypeMessage->taskMutex->priority = ptypeTask->priority;

                /* 如果任务在就绪链表，就需要把任务重新插入到就绪链表的合适位置 */
                if(ptypeMessage->taskMutex->listReady.pNext != &ptypeMessage->taskMutex->listReady)
                {
                    /* 从就绪链表中移除 */
                    cMrListRemove(&ptypeMessage->taskMutex->listReady);

                    /* 提高优先级后，再重新插入到就绪列表 */
                    ptypeMessage->taskMutex->listReady.value = ptypeMessage->taskMutex->priority;
                    cMrListInsert(&g_typeMrListTaskReady, &ptypeMessage->taskMutex->listReady);
                }
            }
        }

        /* 从链表中移除 */
        cMrListRemove(&ptypeTask->listReady);

        /* 插入到消息链表 */
        ptypeTask->listEvent.value = ptypeTask->priority;
        cMrListInsert(&ptypeMessage->listRead, &ptypeTask->listEvent);

        if(lTick > 0LL)
        {
            /* 插入到延时链表 */
            ptypeTask->listDelay.value = lMrTickGet() + lTick;
            cMrListInsert(&g_typeMrListDelay, &ptypeTask->listDelay);
        }
    }

    /* 退出临界段 */
    vMrEnableIsr();

    /* 触发任务切换 */
    vMrCpuYield();

    /* 进入临界段 */
    vMrDisableIsr();

    if(ptypeMessage->index > ptypeMessage->head)
    {
        ptypeMessage->index -= ptypeMessage->size;

        /* 互斥类型 */
        if(ptypeMessage->type == MR_MESSAGE_TYPE_MUTUAL)
        {
            ptypeMessage->taskMutex = ptypeTask;
            ptypeMessage->priorityMutex = ptypeTask->priority;
        }
        /* LIFO类型 */
        else if(ptypeMessage->type == MR_MESSAGE_TYPE_LIFO)
        {
            ptypeMessage->send -= ptypeMessage->size;
            ptypeMessage->send  = ptypeMessage->send < ptypeMessage->head ? (ptypeMessage->tail - ptypeMessage->size) : ptypeMessage->send;

            if(pvData != NULL)
                memcpy(pvData, ptypeMessage->send, ptypeMessage->size);
        }
        /* FIFO类型 */
        else
        {
            if(pvData != NULL)
                memcpy(pvData, ptypeMessage->read, ptypeMessage->size);

            ptypeMessage->read += ptypeMessage->size;
            ptypeMessage->read  = ptypeMessage->read >= ptypeMessage->tail ? ptypeMessage->head : ptypeMessage->read;
        }

        /* 链表中有正在等待的任务 */
        if(ptypeMessage->listSend.pPrev != &ptypeMessage->listSend)
        {
            ptypeTask = (MrTaskType *)ptypeMessage->listSend.pPrev->pElement;

            /* 从链表中移除 */
            cMrListRemove(&ptypeTask->listEvent);
            cMrListRemove(&ptypeTask->listDelay);

            /* 插入到就绪链表 */
            ptypeTask->listReady.value = ptypeTask->priority;
            cMrListInsert(&g_typeMrListTaskReady, &ptypeTask->listReady);
        }

        cError = 0;
    }

    /* 退出临界段 */
    vMrEnableIsr();

    /* 触发任务切换 */
    vMrCpuYield();

    return cError;
}

/* 发送消息 */
int8_t cMrMessageSend(MrMessageType *ptypeMessage, void *pvData, int64_t lTick)
{
    MrTaskType *ptypeTask = (MrTaskType *)g_ptypeMrTaskCurrent;
    int8_t cError = 2;

    if(ptypeMessage == NULL)
        return 1;

    /* 进入临界段 */
    vMrDisableIsr();

    /* 消息队列中消息已满 */
    if((ptypeMessage->index >= ptypeMessage->tail) && (lTick != 0LL))
    {
        /* 从链表中移除 */
        cMrListRemove(&ptypeTask->listReady);

        /* 插入到消息链表 */
        ptypeTask->listEvent.value = ptypeTask->priority;
        cMrListInsert(&ptypeMessage->listSend, &ptypeTask->listEvent);

        if(lTick > 0LL)
        {
            /* 插入到延时链表 */
            ptypeTask->listDelay.value = lMrTickGet() + lTick;
            cMrListInsert(&g_typeMrListDelay, &ptypeTask->listDelay);
        }
    }

    /* 退出临界段 */
    vMrEnableIsr();

    /* 触发任务切换 */
    vMrCpuYield();

    /* 进入临界段 */
    vMrDisableIsr();

    if(ptypeMessage->index < ptypeMessage->tail)
    {
        ptypeMessage->index += ptypeMessage->size;

        /* 互斥类型 */
        if(ptypeMessage->type == MR_MESSAGE_TYPE_MUTUAL)
        {
            /* 恢复优先级 */
            if(ptypeTask->priority != ptypeMessage->priorityMutex)
            {
                ptypeTask->priority = ptypeMessage->priorityMutex;

                /* 从链表中移除 */
                cMrListRemove(&ptypeTask->listReady);

                /* 重新插入到就绪列表 */
                ptypeTask->listReady.value = ptypeTask->priority;
                cMrListInsert(&g_typeMrListTaskReady, &ptypeTask->listReady);
            }
        }

        if(pvData != NULL)
            memcpy(ptypeMessage->send, pvData, ptypeMessage->size);

        ptypeMessage->send += ptypeMessage->size;
        ptypeMessage->send  = ptypeMessage->send >= ptypeMessage->tail ? ptypeMessage->head : ptypeMessage->send;

        /* 链表中有正在等待的任务 */
        if(ptypeMessage->listRead.pPrev != &ptypeMessage->listRead)
        {
            ptypeTask = (MrTaskType *)ptypeMessage->listRead.pPrev->pElement;

            /* 从链表中移除 */
            cMrListRemove(&ptypeTask->listEvent);
            cMrListRemove(&ptypeTask->listDelay);

            /* 插入到就绪链表 */
            ptypeTask->listReady.value = ptypeTask->priority;
            cMrListInsert(&g_typeMrListTaskReady, &ptypeTask->listReady);
        }

        cError = 0;
    }

    /* 退出临界段 */
    vMrEnableIsr();

    /* 触发任务切换 */
    vMrCpuYield();

    return cError;
}
