#ifndef _microMessage_H_
#define _microMessage_H_

#include "stdint.h"


/* FIFO类型 */
#define MR_MESSAGE_TYPE_FIFO    0
/* LIFO类型 */
#define MR_MESSAGE_TYPE_LIFO    1
/* 互斥类型 */
#define MR_MESSAGE_TYPE_MUTUAL  2


/* 消息控制块 */
typedef struct MrMessageStruct{
    MrListType listRead;        /* 读取挂载节点 */
    MrListType listSend;        /* 发送挂载节点 */

    MrTaskType *taskMutex;      /* 互斥信号量 - 占用任务 */
    int32_t priorityMutex;      /* 互斥信号量 - 占用任务的优先级 */

    uint8_t *head;              /* 指针头部 */
    uint8_t *tail;              /* 指针尾部 */
    uint8_t *read;              /* 出队索引 */
    uint8_t *send;              /* 入队索引 */
    uint8_t *index;             /* 当前操作指针 */

    uint16_t type;              /* 类型: FIFO、LIFO */
    int16_t size;               /* 元素大小 */
}MrMessageType;


/* 消息队列 */
int8_t cMrMessageCreate(MrMessageType **pptypeMessage, uint16_t usType, int16_t sSize, int16_t sLength, int16_t sLengthInit);
int8_t cMrMessageRead(MrMessageType *ptypeMessage, void *pvData, int64_t lTick);
int8_t cMrMessageSend(MrMessageType *ptypeMessage, void *pvData, int64_t lTick);
#define cMrMessageSendIsr(ptypeMessage, pvData)                         cMrMessageSend((ptypeMessage), (pvData), 0)

/* 信号量 */
#define cMrMessageSemaphoreCreate(pptypeMessage, sLength, sLengthInit)  cMrMessageCreate((pptypeMessage), MR_MESSAGE_TYPE_FIFO, 1, (sLength), (sLengthInit))
#define cMrMessageSemaphoreRead(ptypeMessage, lTick)                    cMrMessageRead((ptypeMessage), NULL, (lTick))
#define cMrMessageSemaphoreSend(ptypeMessage, lTick)                    cMrMessageSend((ptypeMessage), NULL, (lTick))
#define cMrMessageSemaphoreSendIsr(ptypeMessage)                        cMrMessageSend((ptypeMessage), NULL, 0)

/* 二值信号量 */
#define cMrMessageBinaryCreate(pptypeMessage)                           cMrMessageCreate((pptypeMessage), MR_MESSAGE_TYPE_FIFO, 1, 1, 1)
#define cMrMessageBinaryRead(ptypeMessage, lTick)                       cMrMessageRead((ptypeMessage), NULL, (lTick))
#define cMrMessageBinarySend(ptypeMessage, lTick)                       cMrMessageSend((ptypeMessage), NULL, (lTick))
#define cMrMessageBinarySendIsr(ptypeMessage)                           cMrMessageSend((ptypeMessage), NULL, 0)

/* 互斥信号量 */
#define cMrMessageMutexCreate(pptypeMessage)                            cMrMessageCreate((pptypeMessage), MR_MESSAGE_TYPE_MUTUAL, 1, 1, 1)
#define cMrMessageMutexRead(ptypeMessage, lTick)                        cMrMessageRead((ptypeMessage), NULL, (lTick))
#define cMrMessageMutexSend(ptypeMessage, lTick)                        cMrMessageSend((ptypeMessage), NULL, (lTick))
#define cMrMessageMutexSendIsr(ptypeMessage)                            cMrMessageSend((ptypeMessage), NULL, 0)



#endif // _microMessage_H_
