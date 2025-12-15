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
#define vMrDisableIsr() do{ uint32_t MR_CPU_ISR_STATUS = uiMrCpuDisableIsr()
/* 开启芯片中断 */
#define vMrEnableIsr()  vMrCpuEnableIsr(MR_CPU_ISR_STATUS); }while(0)

uint32_t uiMrCpuDisableIsr(void);
void vMrCpuEnableIsr(uint32_t uiIsr);
void vMrCpuYield(void);
void vMrCpuStart(void);
int8_t cMrCpuTickInit(void);
void vMrCpuSysTickHandler(void);
uint32_t *puiMrTaskStackInit(uint32_t *puiStackTop, void *pvTaskFunction, uint32_t uiParameters);


#endif
