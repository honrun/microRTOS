/*
* Author: honrun
*/
#include "gd32f30x.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "DevicesUart.h"


uint8_t g_USART0ReadDMABuff[USART0_DMA_READ_LENGTH + 4] = {0};
uint8_t g_USART1ReadDMABuff[USART1_DMA_READ_LENGTH + 4] = {0};
uint8_t g_USART2ReadDMABuff[USART2_DMA_READ_LENGTH + 4] = {0};
uint8_t g_USART3ReadDMABuff[USART3_DMA_READ_LENGTH + 4] = {0};


void vUart0Init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USARTx_Tx */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    /* connect port to USARTx_Rx */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);

    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);


    vUart0DMAInit();


    /* USART interrupt configuration */
    nvic_irq_enable(USART0_IRQn, 8, 0);
    /* enable USART interrupt */
    usart_interrupt_enable(USART0, USART_INT_IDLE);


    usart_enable(USART0);
}

void vUart0DMAInit(void)
{
    dma_parameter_struct dma_init_struct = {0};

    /* enable DMAx */
    rcu_periph_clock_enable(RCU_DMA0);

    /* deinitialize DMA channelx(USARTx tx) */
    dma_deinit(DMA0, DMA_CH3);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)NULL;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = USART0_DMA_SEND_LENGTH;
    dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(USART0));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH3, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);
    /* disable DMA channel */
    dma_channel_disable(DMA0, DMA_CH3);

    /* USART DMA enable for transmission */
    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);


    /* deinitialize DMA channelx(USARTx rx) */
    dma_deinit(DMA0, DMA_CH4);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)g_USART0ReadDMABuff;
    dma_init_struct.number = USART0_DMA_READ_LENGTH;
    dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(USART0));
    dma_init(DMA0, DMA_CH4, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_enable(DMA0, DMA_CH4);
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH4);

    /* USART DMA enable for reception */
    usart_dma_receive_config(USART0, USART_DENR_ENABLE);
}


void vUart1Init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOD);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART1);

    /* 引脚重定义 */
    gpio_pin_remap_config(GPIO_USART1_REMAP, ENABLE);

    /* connect port to USARTx_Tx */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    /* connect port to USARTx_Rx */
    gpio_init(GPIOD, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    /* USART configure */
    usart_deinit(USART1);
    usart_baudrate_set(USART1, 115200);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);

    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);


    vUart1DMAInit();


    /* USART interrupt configuration */
    nvic_irq_enable(USART1_IRQn, 7, 0);
    /* enable USART interrupt */
    usart_interrupt_enable(USART1, USART_INT_IDLE);


    usart_enable(USART1);
}

void vUart1DMAInit(void)
{
    dma_parameter_struct dma_init_struct = {0};

    /* enable DMAx */
    rcu_periph_clock_enable(RCU_DMA0);

    /* deinitialize DMA channelx(USARTx tx) */
    dma_deinit(DMA0, DMA_CH6);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)NULL;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = USART1_DMA_SEND_LENGTH;
    dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(USART1));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH6, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH6);
    /* disable DMA channel */
    dma_channel_disable(DMA0, DMA_CH6);

    /* USART DMA enable for transmission */
    usart_dma_transmit_config(USART1, USART_DENT_ENABLE);


    /* deinitialize DMA channelx(USARTx rx) */
    dma_deinit(DMA0, DMA_CH5);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)g_USART1ReadDMABuff;
    dma_init_struct.number = USART1_DMA_READ_LENGTH;
    dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(USART1));
    dma_init(DMA0, DMA_CH5, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_enable(DMA0, DMA_CH5);
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH5);

    /* USART DMA enable for reception */
    usart_dma_receive_config(USART1, USART_DENR_ENABLE);
}


void vUart2Init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOD);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART2);

    /* 引脚重定义 */
    gpio_pin_remap_config(GPIO_USART2_FULL_REMAP, ENABLE);

    /* connect port to USARTx_Tx */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    /* connect port to USARTx_Rx */
    gpio_init(GPIOD, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* USART configure */
    usart_deinit(USART2);
    usart_baudrate_set(USART2, 9600);
    usart_word_length_set(USART2, USART_WL_8BIT);
    usart_stop_bit_set(USART2, USART_STB_1BIT);
    usart_parity_config(USART2, USART_PM_NONE);

    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);


    vUart2DMAInit();


    /* USART interrupt configuration */
    nvic_irq_enable(USART2_IRQn, 8, 0);
    /* enable USART interrupt */
    usart_interrupt_enable(USART2, USART_INT_IDLE);
    usart_interrupt_enable(USART2, USART_INT_RBNE);


    usart_enable(USART2);
}

void vUart2DMAInit(void)
{
    dma_parameter_struct dma_init_struct = {0};

    /* enable DMAx */
    rcu_periph_clock_enable(RCU_DMA0);

    /* deinitialize DMA channelx(USARTx tx) */
    dma_deinit(DMA0, DMA_CH1);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)NULL;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = USART2_DMA_SEND_LENGTH;
    dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(USART2));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH1, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH1);
    /* disable DMA channel */
    dma_channel_disable(DMA0, DMA_CH1);

    /* USART DMA enable for transmission */
    usart_dma_transmit_config(USART2, USART_DENT_ENABLE);


    /* deinitialize DMA channelx(USARTx rx) */
    dma_deinit(DMA0, DMA_CH2);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)g_USART2ReadDMABuff;
    dma_init_struct.number = USART2_DMA_READ_LENGTH;
    dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(USART2));
    dma_init(DMA0, DMA_CH2, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_enable(DMA0, DMA_CH2);
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH2);

    /* USART DMA enable for reception */
    usart_dma_receive_config(USART2, USART_DENR_ENABLE);
}


void vUart3Init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_UART3);

    /* connect port to USARTx_Tx */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    /* connect port to USARTx_Rx */
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    /* USART configure */
    usart_deinit(UART3);
    usart_baudrate_set(UART3, 115200);
    usart_word_length_set(UART3, USART_WL_8BIT);
    usart_stop_bit_set(UART3, USART_STB_1BIT);
    usart_parity_config(UART3, USART_PM_NONE);

    usart_receive_config(UART3, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART3, USART_TRANSMIT_ENABLE);


    vUart3DMAInit();


    /* USART interrupt configuration */
    nvic_irq_enable(UART3_IRQn, 8, 0);
    /* enable USART interrupt */
    usart_interrupt_enable(UART3, USART_INT_IDLE);


    usart_enable(UART3);
}

void vUart3DMAInit(void)
{
    dma_parameter_struct dma_init_struct = {0};

    /* enable DMAx */
    rcu_periph_clock_enable(RCU_DMA1);

    /* deinitialize DMA channelx(USARTx tx) */
    dma_deinit(DMA1, DMA_CH4);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)NULL;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = USART3_DMA_SEND_LENGTH;
    dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(UART3));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA1, DMA_CH4, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA1, DMA_CH4);
    /* disable DMA channel */
    dma_channel_disable(DMA1, DMA_CH4);

    /* USART DMA enable for transmission */
    usart_dma_transmit_config(UART3, USART_DENT_ENABLE);


    /* deinitialize DMA channelx(USARTx rx) */
    dma_deinit(DMA1, DMA_CH2);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)g_USART3ReadDMABuff;
    dma_init_struct.number = USART3_DMA_READ_LENGTH;
    dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(UART3));
    dma_init(DMA1, DMA_CH2, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_enable(DMA1, DMA_CH2);
    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH2);

    /* USART DMA enable for reception */
    usart_dma_receive_config(UART3, USART_DENR_ENABLE);
}


void vUart4Init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_UART4);

    /* connect port to USARTx_Tx */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    /* connect port to USARTx_Rx */
//    gpio_init(GPIOD, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    /* USART configure */
    usart_deinit(UART4);
    usart_baudrate_set(UART4, 115200);
    usart_word_length_set(UART4, USART_WL_8BIT);
    usart_stop_bit_set(UART4, USART_STB_1BIT);
    usart_parity_config(UART4, USART_PM_NONE);

//    usart_receive_config(UART4, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART4, USART_TRANSMIT_ENABLE);


    /* USART interrupt configuration */
//    nvic_irq_enable(UART4_IRQn, 7, 0);
    /* enable USART interrupt */
//    usart_interrupt_enable(UART4, USART_INT_IDLE);


    usart_enable(UART4);
}

void vUartSendDatas(uint32_t uiUsartPeriph, void *pvDatas, int32_t iLength)
{
    uint32_t uiTime = 0;
    uint8_t *pucDatas = pvDatas;

    while((iLength--) > 0)
    {
        uiTime = 1000;
        while((RESET == usart_flag_get(uiUsartPeriph, USART_FLAG_TBE)) && (--uiTime));

        /* Transmit Data */
        USART_DATA(uiUsartPeriph) = *pucDatas++;
    }
}

void vUartSendStrings(uint32_t uiUsartPeriph, char *pcStrings)
{
    vUartSendDatas(uiUsartPeriph, (uint8_t *)pcStrings, strlen(pcStrings));
}


/**
 * @brief  设置DMA的数据传输长度，并启动DMA传输
 * @param  DMAy_Channelx：   相应DMA的通道
           Length：          传输长度
 * @retval 无
 */
void vDMASetAddrAndLenthStart(uint32_t dma_periph, dma_channel_enum channelx, int8_t cDirection, uint32_t uiSourceAddress, uint32_t uiTargetAddress, uint32_t uiLength)
{
    uint32_t uiTime = 0;

    uiTime = 1000000;
    /* Disable the peripheral */
    dma_channel_disable(dma_periph, channelx);
    while(((DMA_CHCTL(dma_periph, channelx) & DMA_CHXCTL_CHEN) != 0) && (--uiTime));

    /* 必须要清除所有中断状态，才允许使能DMA */
    dma_interrupt_flag_clear(dma_periph, channelx, DMA_INT_FLAG_G | DMA_INT_ERR | DMA_INT_HTF | DMA_INT_FTF);

    /* configure memory base address */
    dma_memory_address_config(dma_periph, channelx, uiSourceAddress);

    /* configure peripheral base address */
    dma_periph_address_config(dma_periph, channelx, uiTargetAddress);

    /* Configure DMA Stream data length */
    dma_transfer_number_config(dma_periph, channelx, uiLength);

    /* configure the direction of data transfer on the channel   */
    dma_transfer_direction_config(dma_periph, channelx, cDirection);

    uiTime = 1000000;
    /* Enable the Peripheral */
    dma_channel_enable(dma_periph, channelx);
    while(((DMA_CHCTL(dma_periph, channelx) & DMA_CHXCTL_CHEN) == 0) && (--uiTime));
}

void vUartDMASendDatas(uint32_t uiUsartPeriph, void *pvDatas, int32_t iLength)
{
    dma_channel_enum enumChannelx = DMA_CH1;
    uint32_t dma_periph = 0;
    uint8_t *pucDatas = pvDatas;

    if(iLength <= 0)
        return;

    switch(uiUsartPeriph)
    {
        case USART0: dma_periph = DMA0; enumChannelx = DMA_CH3; break;
        case USART1: dma_periph = DMA0; enumChannelx = DMA_CH6; break;
        case USART2: dma_periph = DMA0; enumChannelx = DMA_CH1; break;
        case UART3:  dma_periph = DMA1; enumChannelx = DMA_CH4; break;

        default : printf("vUartDMASendDatas channel error.\r\n"); return;
    }

    /* 等待上次DMA传输完成 */
    while((dma_flag_get(dma_periph, enumChannelx, DMA_FLAG_FTF) == RESET) && ((DMA_CHCTL(dma_periph, enumChannelx) & DMA_CHXCTL_CHEN) != 0));

    /* 设置传输地址与长度，并启动DMA发送 */
    vDMASetAddrAndLenthStart(dma_periph, enumChannelx, DMA_MEMORY_TO_PERIPHERAL, (uint32_t)pucDatas, (uint32_t)(&USART_DATA(uiUsartPeriph)), iLength);

    /* 等待本次DMA传输完成 */
    while((dma_flag_get(dma_periph, enumChannelx, DMA_FLAG_FTF) == RESET) && ((DMA_CHCTL(dma_periph, enumChannelx) & DMA_CHXCTL_CHEN) != 0));

    /* 等待最后一个字节发送完成 */
    while(RESET == usart_flag_get(uiUsartPeriph, USART_FLAG_TC));
}

void vUartDMASendStrings(uint32_t uiUsartPeriph, char *pcStrings)
{
    vUartDMASendDatas(uiUsartPeriph, (uint8_t *)pcStrings, strlen(pcStrings));
}
