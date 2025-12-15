/*
 * DevicesTimer.c
 *
 *      Author: Honrun
 */
#include "gd32f30x.h"
#include "DevicesTimer.h"


int8_t cTimer5Init(void)
{
    timer_parameter_struct timer_initpara = {0};

    rcu_periph_clock_enable(RCU_TIMER5);

    timer_deinit(TIMER5);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = (SystemCoreClock / 1000000) - 1;
    timer_initpara.period            = 65536 - 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER5, &timer_initpara);


    /* clear channel 0 interrupt bit */
    timer_interrupt_flag_clear(TIMER5, TIMER_INT_FLAG_UP);
    /* channel 0 interrupt enable */
    timer_interrupt_enable(TIMER5, TIMER_INT_FLAG_UP);

    nvic_irq_enable(TIMER5_IRQn, 0, 0);


    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER5);
    /* auto-reload preload enable */
    timer_enable(TIMER5);

    return 0;
}
