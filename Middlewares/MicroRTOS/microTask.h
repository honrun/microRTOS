/*
 *  Author: Honrun
 */
#ifndef _microTask_H_
#define _microTask_H_

#include "stdint.h"


/* 任务函数原型 */
typedef void (* vMrTaskFunction)( size_t );


/* 任务控制块 */
typedef struct MrTaskStruct{
    volatile void *pStack;                  /* 栈指针 */

    MrListType listReady;                   /* 就绪节点 */
    MrListType listEvent;                   /* 事件节点 */
    MrListType listDelay;                   /* 延时节点 */
    char *name;                             /* 任务名 */
    int32_t priority;                       /* 优先级 */
}MrTaskType;


/* 当前正在运行的任务 */
extern volatile MrTaskType *g_ptypeMrTaskCurrent;

/* 空闲任务控制块 */
extern MrTaskType *g_ptypeMrTaskIdle;

/* 定时器任务控制块 */
extern MrTaskType *g_ptypeMrTaskTimer;

/* 就绪任务链表 */
extern MrListType g_typeMrListTaskReady;


void vMrTaskIdle(size_t xParameters);
void vMrTaskError(void);
MrTaskType *ptypeMrTaskReadyHighGet(void);

int8_t cMrTaskCreate(MrTaskType **pptypeTask, vMrTaskFunction pvFunction, char *pcName, int32_t iPriority, int32_t iStackSize, size_t xParameters);
int8_t cMrTaskPrioritySet(MrTaskType *ptypeTask, int32_t priority);
int32_t iMrTaskPriorityGet(MrTaskType *ptypeTask);
int8_t cMrTaskResume(MrTaskType *ptypeTask);
int8_t cMrTaskSuspend(MrTaskType *ptypeTask);
int8_t cMrTaskStart(void);


#endif // _microTask_H_
