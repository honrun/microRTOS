/*
 *  modifier: Honrun
 *  date:     2023/10/29 20:42
 */
#include "ch32x035.h"
#include "stdio.h"
#include "microConfig.h"
#include "microCPU.h"


typedef struct
{
    uint32_t epc;       /* epc - epc    - program counter                     */
    /* RISC-V 使用 x1 寄存器保存子程序的返回地址 */
    uint32_t ra;        /* x1  - ra     - return address for jumps            */
    uint32_t mstatus;   /*              - machine status register             */
    /* RISC-V 使用 x3 寄存器存储全局指针。在 RISC-V 中，全局指针是一个特殊的指针，指向全局变量和静态数据区（static data region）的起始地址 */
    uint32_t gp;        /* x3  - gp     - global pointer                      */
    uint32_t tp;        /* x4  - tp     - thread pointer                      */
    uint32_t t0;        /* x5  - t0     - temporary register 0                */
    uint32_t t1;        /* x6  - t1     - temporary register 1                */
    uint32_t t2;        /* x7  - t2     - temporary register 2                */
    uint32_t s0_fp;     /* x8  - s0/fp  - saved register 0 or frame pointer   */
    uint32_t s1;        /* x9  - s1     - saved register 1                    */
    uint32_t a0;        /* x10 - a0     - return value or function argument 0 */
    uint32_t a1;        /* x11 - a1     - return value or function argument 1 */
    uint32_t a2;        /* x12 - a2     - function argument 2                 */
    uint32_t a3;        /* x13 - a3     - function argument 3                 */
    uint32_t a4;        /* x14 - a4     - function argument 4                 */
    uint32_t a5;        /* x15 - a5     - function argument 5                 */
    uint32_t a6;        /* x16 - a6     - function argument 6                 */
    uint32_t a7;        /* x17 - s7     - function argument 7                 */
    uint32_t s2;        /* x18 - s2     - saved register 2                    */
    uint32_t s3;        /* x19 - s3     - saved register 3                    */
    uint32_t s4;        /* x20 - s4     - saved register 4                    */
    uint32_t s5;        /* x21 - s5     - saved register 5                    */
    uint32_t s6;        /* x22 - s6     - saved register 6                    */
    uint32_t s7;        /* x23 - s7     - saved register 7                    */
    uint32_t s8;        /* x24 - s8     - saved register 8                    */
    uint32_t s9;        /* x25 - s9     - saved register 9                    */
    uint32_t s10;       /* x26 - s10    - saved register 10                   */
    uint32_t s11;       /* x27 - s11    - saved register 11                   */
    uint32_t t3;        /* x28 - t3     - temporary register 3                */
    uint32_t t4;        /* x29 - t4     - temporary register 4                */
    uint32_t t5;        /* x30 - t5     - temporary register 5                */
    uint32_t t6;        /* x31 - t6     - temporary register 6                */

#ifdef ARCH_RISCV_FPU
    uint32_t fx[32];
#endif
}MrCpuStackType;


/* 当前中断嵌套深度 */
volatile int32_t g_iMrCpuIsrDepth = 0;


/* 关闭芯片中断 */
uint32_t uiMrCpuDisableIsr(void)
{
    uint32_t uiIsr;

    __asm volatile ("csrrc %0, mstatus, %1" :"=r"(uiIsr) :"r"(0x88));

    ++g_iMrCpuIsrDepth;

    return uiIsr & 0x88;
}

/* 开启芯片中断 */
void vMrCpuEnableIsr(uint32_t uiIsr)
{
    --g_iMrCpuIsrDepth;

    __asm volatile ("csrrs %0, mstatus, %1" :"=r"(uiIsr) :"r"(uiIsr));
}

/* 触发任务切换 */
void vMrCpuYield(void)
{
    /* 运行的任务跟就绪的最高优先级任务不一致时，才进行切换 */
    if(ptypeMrTaskReadyHighGet() != g_ptypeMrTaskCurrent)
    {
        /* 触发软件中断，以在软件中断函数中执行任务切换 */
        NVIC_SetPendingIRQ(Software_IRQn);
    }
}

/* 系统时钟初始化 */
int8_t cMrCpuTickInit(void)
{
    extern int8_t cTimer2Init(void);
    cTimer2Init();

    /* 设置软件中断优先级，需要跟系统时钟中断优先级设置成一样 */
    NVIC_SetPriority(Software_IRQn, 0xF0);
    NVIC_ClearPendingIRQ(Software_IRQn);
    NVIC_EnableIRQ(Software_IRQn);

    return 0;
}

/* 系统心跳（需要以设定的RTOS频率定时调用） */
void vMrCpuSysTickHandler(void)
{
    /* 系统时钟递增 */
    vMrTickGather();

    /* 延时判断 */
    vMrDelayCheck();

    /* 触发任务切换 */
    vMrCpuYield();
}

/* 任务栈初始化 */
uint32_t *puiMrTaskStackInit(uint32_t *puiStackTop, void *pvTaskFunction, uint32_t uiParameters)
{
    MrCpuStackType *frame = (MrCpuStackType *)((uint32_t)puiStackTop - sizeof(MrCpuStackType));

    frame->ra   = (uint32_t)vMrTaskError;
    frame->a0   = (uint32_t)uiParameters;
    frame->epc  = (uint32_t)pvTaskFunction;

    /* set MPIE to 1，以在开启任务时，自动开启全局中断 */
#ifdef ARCH_RISCV_FPU
    frame->mstatus = 0x7880;
#else
    frame->mstatus = 0x1880;
#endif

    return (uint32_t *)frame;
}
