/*
* Author: honrun
*/
#ifndef DEVICESUART_H_
#define DEVICESUART_H_


#define UART_LOG        USART0
#define UART_BUS        USART1
#define UART_433        USART2
#define UART_MASTER1    USART2
#define UART_MASTER2    UART3


#define USART0_DMA_READ_LENGTH  (512)
#define USART0_DMA_SEND_LENGTH  (0)
#define USART1_DMA_READ_LENGTH  (512)
#define USART1_DMA_SEND_LENGTH  (0)
#define USART2_DMA_READ_LENGTH  (512)
#define USART2_DMA_SEND_LENGTH  (0)
#define USART3_DMA_READ_LENGTH  (512)
#define USART3_DMA_SEND_LENGTH  (0)



extern uint8_t g_USART0ReadDMABuff[USART0_DMA_READ_LENGTH + 4];
extern uint8_t g_USART1ReadDMABuff[USART1_DMA_READ_LENGTH + 4];
extern uint8_t g_USART2ReadDMABuff[USART2_DMA_READ_LENGTH + 4];
extern uint8_t g_USART3ReadDMABuff[USART3_DMA_READ_LENGTH + 4];



void vUart0Init(void);
void vUart1Init(void);
void vUart2Init(void);
void vUart3Init(void);
void vUart4Init(void);


void vUart0DMAInit(void);
void vUart1DMAInit(void);
void vUart2DMAInit(void);
void vUart3DMAInit(void);


void vUartSendDatas(uint32_t uiUsartPeriph, void *pvDatas, int32_t iLength);
void vUartSendStrings(uint32_t uiUsartPeriph, char *pcStrings);

void vUartDMASendDatas(uint32_t uiUsartPeriph, void *pvDatas, int32_t iLength);
void vUartDMASendStrings(uint32_t uiUsartPeriph, char *pcStrings);

int8_t cUartReceiveByte(uint32_t uiUsartPeriph, uint8_t *pucByte);
int8_t cUartReceiveDatas(uint32_t uiUsartPeriph, void *pvDatas, int32_t iLength);
int8_t cUartReceiveAllDatas(uint32_t uiUsartPeriph, void *pvDatas, int32_t *piLength);
int8_t cUartReceiveLengthGet(uint32_t uiUsartPeriph, int32_t *piOutLength);


#endif /* DEVICESUART_H_ */
