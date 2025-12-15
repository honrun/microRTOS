/*
 *  modifier: Honrun
 *  date:     2023/10/29 20:42
 */
#include "gd32f30x.h"
#include "stdio.h"
#include "microConfig.h"
#include "microCPU.h"


/* 当前中断嵌套深度 */
volatile int32_t g_iMrCpuIsrDepth = 0;


/* 关闭芯片中断 */
uint32_t uiMrCpuDisableIsr(void)
{
    __set_BASEPRI(configMr_PRIORITY_SYSTEM_INTERRUPT_MAX);

    ++g_iMrCpuIsrDepth;

    return g_iMrCpuIsrDepth;
}

/* 开启芯片中断 */
void vMrCpuEnableIsr(uint32_t uiIsr)
{
    /* 中断嵌套全部退出时，再把中断屏蔽位设置为0 */
    if((--g_iMrCpuIsrDepth) <= 0)
    {
        g_iMrCpuIsrDepth = 0;

        __set_BASEPRI(0);
    }
}

/* 触发任务切换 */
void vMrCpuYield(void)
{
    /* 运行的任务跟就绪的最高优先级任务不一致时，才进行切换 */
    if(ptypeMrTaskReadyHighGet() != g_ptypeMrTaskCurrent)
    {
        /* 触发 PendSV 中断，以进行任务切换 */
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    }
}

void vMrCpuSysTickHandler( void )
{
    /* 系统时钟递增 */
    vMrTickGather();

    /* 延时判断 */
    vMrDelayCheck();

    /* 触发任务切换 */
    vMrCpuYield();
}

void vMrCpuPendSVHandler( void )
{
    __asm volatile
    (
        "mrs r0, psp                            \n"
        "isb                                    \n"
        "                                       \n"
        "ldr r3, =g_ptypeMrTaskCurrent          \n"/* Get the location of the current TCB. */
        "ldr r2, [r3]                           \n"
        "                                       \n"
        "stmdb r0!, {r4-r11}                    \n"/* Save the remaining registers. */
        "str r0, [r2]                           \n"/* Save the new top of stack into the first member of the TCB. */
        "                                       \n"
        "stmdb sp!, {r3, r14}                   \n"
        "mov r0, %0                             \n"
        "msr basepri, r0                        \n"
        "bl vMrTaskSwitchContext                \n"
        "mov r0, #0                             \n"
        "msr basepri, r0                        \n"
        "ldmia sp!, {r3, r14}                   \n"
        "                                       \n"/* Restore the context, including the critical nesting count. */
        "ldr r1, [r3]                           \n"
        "ldr r0, [r1]                           \n"/* The first item in pxCurrentTCB is the task top of stack. */
        "ldmia r0!, {r4-r11}                    \n"/* Pop the registers. */
        "msr psp, r0                            \n"
        "isb                                    \n"
        "bx r14                                 \n"
        "                                       \n"
        ".align 4                               \n"
        ::"i" ( configMr_PRIORITY_SYSTEM_INTERRUPT_MAX )
    );
}

void vMrCpuSVCHandler( void )
{
    __asm volatile
    (
        "ldr r3, =g_ptypeMrTaskCurrent          \n"/* Restore the context. */
        "ldr r1, [r3]                           \n"/* Use pxCurrentTCBConst to get the pxCurrentTCB address. */
        "ldr r0, [r1]                           \n"/* The first item in pxCurrentTCB is the task top of stack. */
        "ldmia r0!, {r4-r11}                    \n"/* Pop the registers that are not automatically saved on exception entry and the critical nesting count. */
        "msr psp, r0                            \n"/* Restore the task stack pointer. */
        "isb                                    \n"
        "mov r0, #0                             \n"
        "msr basepri, r0                        \n"
        "orr r14, #0xd                          \n"
        "bx r14                                 \n"
    );
}

/* 启动第一个任务 */
void vMrCpuStart( void )
{
    __asm volatile
    (
        "cpsie i                                \n"/* Globally enable interrupts. */
        "cpsie f                                \n"
        "dsb                                    \n"
        "isb                                    \n"
        "svc 0                                  \n"/* System call to start first task. */
        "nop                                    \n"
    );
}

/* 线程模式 */
#define cpuINITIAL_XPSR     ( 0x01000000 )

/* 任务栈初始化 */
uint32_t *puiMrTaskStackInit(uint32_t *puiStackTop, void *pvTaskFunction, uint32_t uiParameters)
{
    /* Simulate the stack frame as it would be created by a context switch interrupt. */
    /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
    puiStackTop--;
    *puiStackTop = cpuINITIAL_XPSR;             /* xPSR */
    puiStackTop--;
    *puiStackTop = (uint32_t)pvTaskFunction;    /* PC */
    puiStackTop--;
    *puiStackTop = (uint32_t)vMrTaskError;      /* LR */
    puiStackTop -= 5;                           /* R12, R3, R2 and R1. */
    *puiStackTop = uiParameters;                /* R0 */
    puiStackTop -= 8;                           /* R11, R10, R9, R8, R7, R6, R5 and R4. */

    return puiStackTop;
}

/* 系统时钟初始化 */
int8_t cMrCpuTickInit(void)
{
    /* Disable SysTick IRQ and SysTick Timer */
    SysTick->CTRL = 0;
    /* Load the SysTick Counter Value */
    SysTick->VAL = 0;
    /* Set the SysTick Counter Load */
    SysTick->LOAD = ( configMr_CPU_CLOCK_HZ / configMr_RTOS_CLOCK_HZ ) - 1UL;

    /* set Priority for Systick and pendsv Interrupt */
    NVIC_SetPriority (SysTick_IRQn, 15);
    NVIC_SetPriority (PendSV_IRQn, 15);

    /* Enable SysTick IRQ and SysTick Timer */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

    return 0;
}
