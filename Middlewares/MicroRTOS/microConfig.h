/*
 *  modifier: Honrun
 *  date:     2023/10/29 20:42
 */
#ifndef _microConfig_H_
#define _microConfig_H_

#include "stdint.h"


/* 芯片时钟频率 */
#define configMr_CPU_CLOCK_HZ                   ((uint32_t )120000000)
/* RTOS运行频率 */
#define configMr_RTOS_CLOCK_HZ                  ((uint32_t )1000)
/* RTOS 内存大小 */
#define configMr_MEMORY_SIZE                    (1024 * 12)
/* RTOS 任务栈最小值 */
#define configMr_TASK_STACK_MIN_SIZE            512
/* 软件定时器 任务栈大小 */
#define configMr_TASK_STACK_TIMER_SIZE          1024
/* 定时器任务优先级 */
#define configMr_TASK_PRIORITY_TIMER            -1
/* 系统中断可屏蔽优化级阈值 */
#define configMr_PRIORITY_SYSTEM_INTERRUPT_MAX  0x5F


#include "microMemory.h"
#include "microTime.h"
#include "microList.h"
#include "microCPU.h"
#include "microDelay.h"
#include "microTask.h"
#include "microMessage.h"
#include "microTimer.h"


#endif // _microConfig_H_
