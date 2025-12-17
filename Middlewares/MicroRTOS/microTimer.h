#ifndef _microTimer_H_
#define _microTimer_H_

#include "stdint.h"


/* 定时器回调函数原型 */
typedef void (* vMrTimerFunction)( size_t );


typedef enum {
    MrTimerStop = 0x00,             /* 定时器已关闭 */
    MrTimerOpen = 0x01,             /* 正常计时当中 */
    MrTimerCirculation = 0x04,      /* 循环模式 */
    MrTimerImmediately = 0x08,      /* 创建后，立即执行一次 */
    MrTimerError = 0x80,            /* 定时器错误 */
}MrTimerStateEnum;

typedef struct MrTimerStruct{
    MrListType list;                /* 节点 */
    int64_t duration;               /* 定时时长（Tick） */
    vMrTimerFunction pFunction;     /* 回调函数 */
    size_t parameters;              /* 参数 */
    MrTimerStateEnum state;         /* 状态 */
}MrTimerType;


int8_t cMrTimerCreate(MrTimerType **pptypeTimer, vMrTimerFunction pvFunction, size_t xParameters, int64_t lTick);
int8_t cMrTimerStart(MrTimerType *ptypeTimer, MrTimerStateEnum enumState);
int8_t cMrTimerStop(MrTimerType *ptypeTimer);
void vMrTaskTimer(size_t xParameters);


#endif // _microTimer_H_
