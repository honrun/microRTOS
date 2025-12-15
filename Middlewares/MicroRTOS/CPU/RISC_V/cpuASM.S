/*
* License:      MIT
* Author        Date            Change
* Honrun        2024/11/30      First update
*/
#define portWORD_SIZE           4


.extern g_ptypeMrTaskCurrent
.extern vMrTaskSwitchContext


.func
.align 8

/*
 * 切换到系统栈
 * void vMrStackSwitchToSystem(void);
 */
.globl vMrStackSwitchToSystem
vMrStackSwitchToSystem:
    /* 系统栈缓存 -> t0 */
    csrr t0, mscratch
    j imm_l

/*
 * 切换到任务栈
 * void vMrStackSwitchToTask(void);
 */
.globl vMrStackSwitchToTask
vMrStackSwitchToTask:
    /* 实时栈 -> t0 */
    mv t0, sp

imm_l:
    /* 系统栈顶 -> t1 */
    la t1, _eusrstack
    /* 如果 系统栈缓存 不等于 系统栈顶，表示已经切换到系统栈 */
    /* 如果 实时栈     不等于 系统栈顶，表示系统栈还未全部出栈 */
    bne t0, t1, imm_2
    /* 切换栈 */
    csrrw sp, mscratch, sp
imm_2:
    ret

/*
 * 开启任务调度
 * void vMrCpuStart(void);
 */
.global vMrCpuStart
vMrCpuStart:
    /* 保存系统栈顶（后续系统栈、任务栈会利用 mscratch 进行相互切换） */
    la t0, _eusrstack
    csrw mscratch, t0

    j SW_Handler_Exit


.global SW_Handler
SW_Handler:
#ifdef ARCH_RISCV_FPU
    addi sp, sp, (-32 * portWORD_SIZE)
    fsw f0,   0 * portWORD_SIZE( sp )
    fsw f1,   1 * portWORD_SIZE( sp )
    fsw f2,   2 * portWORD_SIZE( sp )
    fsw f3,   3 * portWORD_SIZE( sp )
    fsw f4,   4 * portWORD_SIZE( sp )
    fsw f5,   5 * portWORD_SIZE( sp )
    fsw f6,   6 * portWORD_SIZE( sp )
    fsw f7,   7 * portWORD_SIZE( sp )
    fsw f8,   8 * portWORD_SIZE( sp )
    fsw f9,   9 * portWORD_SIZE( sp )
    fsw f10, 10 * portWORD_SIZE( sp )
    fsw f11, 11 * portWORD_SIZE( sp )
    fsw f12, 12 * portWORD_SIZE( sp )
    fsw f13, 13 * portWORD_SIZE( sp )
    fsw f14, 14 * portWORD_SIZE( sp )
    fsw f15, 15 * portWORD_SIZE( sp )
    fsw f16, 16 * portWORD_SIZE( sp )
    fsw f17, 17 * portWORD_SIZE( sp )
    fsw f18, 18 * portWORD_SIZE( sp )
    fsw f19, 19 * portWORD_SIZE( sp )
    fsw f20, 20 * portWORD_SIZE( sp )
    fsw f21, 21 * portWORD_SIZE( sp )
    fsw f22, 22 * portWORD_SIZE( sp )
    fsw f23, 23 * portWORD_SIZE( sp )
    fsw f24, 24 * portWORD_SIZE( sp )
    fsw f25, 25 * portWORD_SIZE( sp )
    fsw f26, 26 * portWORD_SIZE( sp )
    fsw f27, 27 * portWORD_SIZE( sp )
    fsw f28, 28 * portWORD_SIZE( sp )
    fsw f29, 29 * portWORD_SIZE( sp )
    fsw f30, 30 * portWORD_SIZE( sp )
    fsw f31, 31 * portWORD_SIZE( sp )
#endif

    addi sp, sp, (-32 * portWORD_SIZE)
    sw x1,   1 * portWORD_SIZE( sp )
    /* x2(sp)，会保存在任务控制块 */
    /* x3(gp)，不需要保存全局指针 */
    sw x4,   4 * portWORD_SIZE( sp )
    sw x5,   5 * portWORD_SIZE( sp )
    sw x6,   6 * portWORD_SIZE( sp )
    sw x7,   7 * portWORD_SIZE( sp )
    sw x8,   8 * portWORD_SIZE( sp )
    sw x9,   9 * portWORD_SIZE( sp )
    sw x10, 10 * portWORD_SIZE( sp )
    sw x11, 11 * portWORD_SIZE( sp )
    sw x12, 12 * portWORD_SIZE( sp )
    sw x13, 13 * portWORD_SIZE( sp )
    sw x14, 14 * portWORD_SIZE( sp )
    sw x15, 15 * portWORD_SIZE( sp )
    sw x16, 16 * portWORD_SIZE( sp )
    sw x17, 17 * portWORD_SIZE( sp )
    sw x18, 18 * portWORD_SIZE( sp )
    sw x19, 19 * portWORD_SIZE( sp )
    sw x20, 20 * portWORD_SIZE( sp )
    sw x21, 21 * portWORD_SIZE( sp )
    sw x22, 22 * portWORD_SIZE( sp )
    sw x23, 23 * portWORD_SIZE( sp )
    sw x24, 24 * portWORD_SIZE( sp )
    sw x25, 25 * portWORD_SIZE( sp )
    sw x26, 26 * portWORD_SIZE( sp )
    sw x27, 27 * portWORD_SIZE( sp )
    sw x28, 28 * portWORD_SIZE( sp )
    sw x29, 29 * portWORD_SIZE( sp )
    sw x30, 30 * portWORD_SIZE( sp )
    sw x31, 31 * portWORD_SIZE( sp )

    /* 保存 PC 值 */
    csrr t0, mepc
    sw t0,   0 * portWORD_SIZE( sp )

    /* 保存 mstatus 值 */
    csrr t0, mstatus
    sw t0,   2 * portWORD_SIZE( sp )

    /* 更新任务控制块结构体里头的栈指针值 */
    lw t0,  g_ptypeMrTaskCurrent
    sw sp,  0(t0)

    jal vMrStackSwitchToSystem
    /* 切换到就绪的最高优先级任务 */
    jal vMrTaskSwitchContext
    jal vMrStackSwitchToTask

SW_Handler_Exit:
    /* 切换到新任务栈 */
    lw t0,   g_ptypeMrTaskCurrent
    lw sp,   0( t0 )

    /* 返回地址，需要以 mret形式 返回 */
    lw t0,   0 * portWORD_SIZE( sp )
    csrw mepc, t0

    /* 恢复任务 mstatus，以在此退出时开启全局中断 */
    lw t0,   2 * portWORD_SIZE( sp )
    csrw mstatus, t0

    lw x1,   1 * portWORD_SIZE( sp )
    lw x4,   4 * portWORD_SIZE( sp )
    lw x5,   5 * portWORD_SIZE( sp )
    lw x6,   6 * portWORD_SIZE( sp )
    lw x7,   7 * portWORD_SIZE( sp )
    lw x8,   8 * portWORD_SIZE( sp )
    lw x9,   9 * portWORD_SIZE( sp )
    lw x10, 10 * portWORD_SIZE( sp )
    lw x11, 11 * portWORD_SIZE( sp )
    lw x12, 12 * portWORD_SIZE( sp )
    lw x13, 13 * portWORD_SIZE( sp )
    lw x14, 14 * portWORD_SIZE( sp )
    lw x15, 15 * portWORD_SIZE( sp )
    lw x16, 16 * portWORD_SIZE( sp )
    lw x17, 17 * portWORD_SIZE( sp )
    lw x18, 18 * portWORD_SIZE( sp )
    lw x19, 19 * portWORD_SIZE( sp )
    lw x20, 20 * portWORD_SIZE( sp )
    lw x21, 21 * portWORD_SIZE( sp )
    lw x22, 22 * portWORD_SIZE( sp )
    lw x23, 23 * portWORD_SIZE( sp )
    lw x24, 24 * portWORD_SIZE( sp )
    lw x25, 25 * portWORD_SIZE( sp )
    lw x26, 26 * portWORD_SIZE( sp )
    lw x27, 27 * portWORD_SIZE( sp )
    lw x28, 28 * portWORD_SIZE( sp )
    lw x29, 29 * portWORD_SIZE( sp )
    lw x30, 30 * portWORD_SIZE( sp )
    lw x31, 31 * portWORD_SIZE( sp )
    addi sp, sp, (32 * portWORD_SIZE)

#ifdef ARCH_RISCV_FPU
    flw f0,   0 * portWORD_SIZE( sp )
    flw f1,   1 * portWORD_SIZE( sp )
    flw f2,   2 * portWORD_SIZE( sp )
    flw f3,   3 * portWORD_SIZE( sp )
    flw f4,   4 * portWORD_SIZE( sp )
    flw f5,   5 * portWORD_SIZE( sp )
    flw f6,   6 * portWORD_SIZE( sp )
    flw f7,   7 * portWORD_SIZE( sp )
    flw f8,   8 * portWORD_SIZE( sp )
    flw f9,   9 * portWORD_SIZE( sp )
    flw f10, 10 * portWORD_SIZE( sp )
    flw f11, 11 * portWORD_SIZE( sp )
    flw f12, 12 * portWORD_SIZE( sp )
    flw f13, 13 * portWORD_SIZE( sp )
    flw f14, 14 * portWORD_SIZE( sp )
    flw f15, 15 * portWORD_SIZE( sp )
    flw f16, 16 * portWORD_SIZE( sp )
    flw f17, 17 * portWORD_SIZE( sp )
    flw f18, 18 * portWORD_SIZE( sp )
    flw f19, 19 * portWORD_SIZE( sp )
    flw f20, 20 * portWORD_SIZE( sp )
    flw f21, 21 * portWORD_SIZE( sp )
    flw f22, 22 * portWORD_SIZE( sp )
    flw f23, 23 * portWORD_SIZE( sp )
    flw f24, 24 * portWORD_SIZE( sp )
    flw f25, 25 * portWORD_SIZE( sp )
    flw f26, 26 * portWORD_SIZE( sp )
    flw f27, 27 * portWORD_SIZE( sp )
    flw f28, 28 * portWORD_SIZE( sp )
    flw f29, 29 * portWORD_SIZE( sp )
    flw f30, 30 * portWORD_SIZE( sp )
    flw f31, 31 * portWORD_SIZE( sp )
    addi sp, sp, (32 * portWORD_SIZE)
#endif

    mret
