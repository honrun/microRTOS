/*
 *  Author: Honrun
 */
#ifndef _microCPU_H_
#define _microCPU_H_

#include "stdint.h"


/* 此两个函数需要成对使用 */
void vMrStackSwitchToSystem(void);
void vMrStackSwitchToTask(void);

/* 关闭芯片中断 */
#define vMrDisableIsr() do{ size_t MR_CPU_ISR_STATUS = xMrCpuDisableIsr()
/* 开启芯片中断 */
#define vMrEnableIsr()  vMrCpuEnableIsr(MR_CPU_ISR_STATUS); }while(0)

size_t xMrCpuDisableIsr(void);
void vMrCpuEnableIsr(size_t uiIsr);
void vMrCpuYield(void);
void vMrCpuStart(void);
int8_t cMrCpuTickInit(void);
void vMrCpuSysTickHandler(void);
void *pvMrTaskStackInit(void *pvStackTop, void *pvTaskFunction, size_t xParameters);


#endif
