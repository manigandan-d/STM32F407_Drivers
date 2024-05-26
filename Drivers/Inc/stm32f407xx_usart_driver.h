#ifndef INC_STM32F407XX_USART_DRIVER_H_
#define INC_STM32F407XX_USART_DRIVER_H_

#include "stm32f407xx.h"

typedef struct {
	uint8_t USART_Mode;
	uint32_t USART_Baud;
	uint8_t USART_NoOfStopBits;
	uint8_t USART_WordLen;
	uint8_t USART_ParityCtrl;
	uint8_t USART_HWFlowCtrl;
}USART_Config_t;

typedef struct {
	USART_RegDef_t *pUSARTx;
	USART_Config_t USART_Config;
	uint8_t *pTxBuffer;
	uint8_t *pRxBuffer;
	uint32_t TxLen;
	uint32_t RxLen;
	uint8_t TxBusyState;
	uint8_t RxBusyState;
}USART_Handle_t;

#define USART_MODE_TX_ONLY 0
#define USART_MODE_RX_ONLY 1
#define USART_MODE_TXRX 2

#define USART_BAUD_1200 1200
#define USART_BAUD_2400 2400
#define USART_BAUD_9600 9600
#define USART_BAUD_19200 19200
#define USART_BAUD_38400 38400
#define USART_BAUD_57600 57600
#define USART_BAUD_115200 115200
#define USART_BAUD_230400 230400
#define USART_BAUD_460800 460800
#define USART_BAUD_921600 921600
#define USART_BAUD_2M 2000000
#define USART_BAUD_3M 3000000

#define USART_STOPBITS_1 0
#define USART_STOPBITS_0_5 1
#define USART_STOPBITS_2 2
#define USART_STOPBITS_1_5 3

#define USART_WORDLEN_8BIT 0
#define USART_WORDLEN_9BIT 1

#define USART_PARITY_DI 0
#define USART_PARITY_EN_ODD 1
#define USART_PARITY_EN_EVEN 2

#define USART_HWFLOWCTRL_NONE 0
#define USART_HWFLOWCTRL_CTS 1
#define USART_HWFLOWCTRL_RTS 2
#define USART_HWFLOWCTRL_CTS_RTS 3

#define USART_FLAG_PE (1 << USART_SR_PE)
#define USART_FLAG_FE (1 << USART_SR_FE)
#define USART_FLAG_NF (1 << USART_SR_NF)
#define USART_FLAG_ORE (1 << USART_SR_ORE)
#define USART_FLAG_IDLE (1 << USART_SR_IDLE)
#define USART_FLAG_RXNE (1 << USART_SR_RXNE)
#define USART_FLAG_TC (1 << USART_SR_TC)
#define USART_FLAG_TXE (1 << USART_SR_TXE)
#define USART_FLAG_LBD (1 << USART_SR_LBD)
#define USART_FLAG_CTS (1 << USART_SR_CTS)

#define USART_READY 0
#define USART_BUSY_IN_RX 1
#define USART_BUSY_IN_TX 2

#define USART_EV_TX_CMPLT 0
#define USART_EV_RX_CMPLT 1
#define USART_EV_IDLE 2
#define USART_EV_CTS 3
#define USART_ER_PE 4
#define USART_ER_FE 5
#define USART_ER_NE 6
#define USART_ER_ORE 7

void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi);
void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi);
void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t baudRate);

void USART_Init(USART_Handle_t *pUSARTHandle);
void USART_DeInit(USART_RegDef_t *pUSARTx);

uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint32_t flagName);

void USART_SendData(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t len);
void USART_ReceiveData(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t len);

uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t len);
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t len);

void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void USART_IRQHandling(USART_Handle_t *pUSARTHandle);

void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle, uint8_t ApEv);

#endif /* INC_STM32F407XX_USART_DRIVER_H_ */
