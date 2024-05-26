#include "stm32f407xx_usart_driver.h"

void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		if(pUSARTx == USART1) {
			USART1_PCLK_EN();
		}

		else if(pUSARTx == USART2) {
			USART2_PCLK_EN();
		}

		else if(pUSARTx == USART3) {
			USART3_PCLK_EN();
		}

		else if(pUSARTx == UART4) {
			UART4_PCLK_EN();
		}

		else if(pUSARTx == UART5) {
			UART5_PCLK_EN();
		}

		else if(pUSARTx == USART6) {
			USART6_PCLK_EN();
		}
	}

	else {
		if(pUSARTx == USART1) {
			USART1_PCLK_DI();
		}

		else if(pUSARTx == USART2) {
			USART2_PCLK_DI();
		}

		else if(pUSARTx == USART3) {
			USART3_PCLK_DI();
		}

		else if(pUSARTx == UART4) {
			UART4_PCLK_DI();
		}

		else if(pUSARTx == UART5) {
			UART5_PCLK_DI();
		}

		else if(pUSARTx == USART6) {
			USART6_PCLK_DI();
		}
	}
}

void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		pUSARTx->CR1 |= (1 << USART_CR1_UE);
	}

	else {
		pUSARTx->CR1 &= ~(1 << USART_CR1_UE);
	}
}

void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t baudRate) {
	uint32_t PCLKx;
	uint32_t usartDiv;
	uint32_t mantissaPart, fractionPart;

	uint32_t tempReg = 0;

	if(pUSARTx == USART1 || pUSARTx == USART6) {
		PCLKx = RCC_GetPCLK2Value();
	}

	else {
		PCLKx = RCC_GetPCLK1Value();
	}

	if(pUSARTx->CR1 & (1 << USART_CR1_OVER8)) {
		usartDiv = (25 * PCLKx) / (2 * baudRate);
	}

	else {
		usartDiv = (25 * PCLKx) / (4 * baudRate);
	}

	mantissaPart = usartDiv / 100;

	tempReg = mantissaPart << 4;

	fractionPart = (usartDiv - (mantissaPart * 100));

	if(pUSARTx->CR1 & (1 << USART_CR1_OVER8)) {
		fractionPart = (((fractionPart * 8) + 50) / 100) & (uint8_t)0x07;
	}

	else {
		fractionPart = (((fractionPart * 16) + 50) / 100) & (uint8_t)0x0F;
	}

	tempReg |= fractionPart;

	pUSARTx->BRR = tempReg;
}

void USART_Init(USART_Handle_t *pUSARTHandle) {
	uint32_t tempReg=0;

	USART_PeriClockControl(pUSARTHandle->pUSARTx, ENABLE);

	if(pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TX_ONLY) {
		tempReg = 1 << USART_CR1_TE;
	}

	else if(pUSARTHandle->USART_Config.USART_Mode == USART_MODE_RX_ONLY) {
		tempReg = 1 << USART_CR1_RE;
	}

	else if(pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TXRX) {
		tempReg = 1 << USART_CR1_TE;
		tempReg |= (1 << USART_CR1_RE);
	}

	tempReg |= (pUSARTHandle->USART_Config.USART_WordLen << USART_CR1_M);

	if(pUSARTHandle->USART_Config.USART_ParityCtrl == USART_PARITY_EN_EVEN) {
		tempReg |= (1 << USART_CR1_PCE);
	}

	else if(pUSARTHandle->USART_Config.USART_ParityCtrl == USART_PARITY_EN_ODD) {
		tempReg |= (1 << USART_CR1_PCE);
		tempReg |= (1 << USART_CR1_PS);
	}

	pUSARTHandle->pUSARTx->CR1 = tempReg;

	tempReg = pUSARTHandle->USART_Config.USART_NoOfStopBits << USART_CR2_STOP;
	pUSARTHandle->pUSARTx->CR2 = tempReg;

	if(pUSARTHandle->USART_Config.USART_HWFlowCtrl == USART_HWFLOWCTRL_CTS) {
		tempReg = 1 << USART_CR3_CTSE;
	}

	else if(pUSARTHandle->USART_Config.USART_HWFlowCtrl == USART_HWFLOWCTRL_RTS) {
		tempReg = 1 << USART_CR3_RTSE;
	}

	else if(pUSARTHandle->USART_Config.USART_HWFlowCtrl == USART_HWFLOWCTRL_CTS_RTS) {
		tempReg = 1 << USART_CR3_CTSE;
		tempReg |= (1 << USART_CR3_RTSE);
	}

	pUSARTHandle->pUSARTx->CR3 = tempReg;

	USART_SetBaudRate(pUSARTHandle->pUSARTx, pUSARTHandle->USART_Config.USART_Baud);
}

void USART_DeInit(USART_RegDef_t *pUSARTx) {
	if(pUSARTx == USART1) {
		USART1_REG_RESET();
	}

	else if(pUSARTx == USART2) {
		USART2_REG_RESET();
	}

	else if(pUSARTx == USART3) {
		USART3_REG_RESET();
	}

	else if(pUSARTx == UART4) {
		UART4_REG_RESET();
	}

	else if(pUSARTx == UART5) {
		UART5_REG_RESET();
	}

	else if(pUSARTx == USART6) {
		USART6_REG_RESET();
	}
}

uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint32_t flagName) {
	if(pUSARTx->SR & flagName) {
		return FLAG_SET;
	}

	return FLAG_RESET;
}

void USART_SendData(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t len) {
	uint16_t *pdata;

	for(uint32_t i=0; i<len; i++) {
		while(!USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TXE));

		if(pUSARTHandle->USART_Config.USART_WordLen == USART_WORDLEN_9BIT) {
			pdata = (uint16_t *)pTxBuffer;

			pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FF);

			if(pUSARTHandle->USART_Config.USART_ParityCtrl == USART_PARITY_DI) {
				pTxBuffer++;
				pTxBuffer++;
			}

			else {
				pTxBuffer++;
			}
		}

		else {
			pUSARTHandle->pUSARTx->DR = *pTxBuffer;

			pTxBuffer++;
		}
	}

	while(!USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TC));
}

void USART_ReceiveData(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t len) {
	for(uint32_t i=0; i<len; i++) {
		while(!USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_RXNE));

		if(pUSARTHandle->USART_Config.USART_WordLen == USART_WORDLEN_9BIT) {
			if(pUSARTHandle->USART_Config.USART_ParityCtrl == USART_PARITY_DI) {
				*((uint16_t *)pRxBuffer) = pUSARTHandle->pUSARTx->DR & (uint16_t)0x01FF;

				pRxBuffer++;
				pRxBuffer++;
			}

			else {
				*pRxBuffer = pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF;

				pRxBuffer++;
			}
		}

		else {
			if(pUSARTHandle->USART_Config.USART_ParityCtrl == USART_PARITY_DI) {
				*pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF);
			}

			else {
				*pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0x7F);
			}

			pRxBuffer++;
		}
	}
}

uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t len) {
	uint8_t TxState = pUSARTHandle->TxBusyState;

	if(TxState != USART_BUSY_IN_TX) {
		pUSARTHandle->TxLen = len;
		pUSARTHandle->pTxBuffer = pTxBuffer;
		pUSARTHandle->TxBusyState = USART_BUSY_IN_TX;

		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TXEIE);
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TCIE);
	}

	return TxState;
}

uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t len) {
	uint8_t RxState = pUSARTHandle->RxBusyState;

	if(RxState != USART_BUSY_IN_RX) {
		pUSARTHandle->RxLen = len;
		pUSARTHandle->pRxBuffer = pRxBuffer;
		pUSARTHandle->RxBusyState = USART_BUSY_IN_RX;

		(void)pUSARTHandle->pUSARTx->DR;

		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_RXNEIE);
	}

	return RxState;
}

void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		if(IRQNumber <= 31) {
			*NVIC_ISER0 |= (1 << IRQNumber);
		}

		else if(IRQNumber > 31 && IRQNumber < 64) {
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));
		}

		else if(IRQNumber >= 64 && IRQNumber < 96) {
			*NVIC_ISER2 |= (1 << (IRQNumber % 64));
		}
	}

	else {
		if(IRQNumber <= 31) {
			*NVIC_ICER0 |= (1 << IRQNumber);
		}

		else if(IRQNumber > 31 && IRQNumber < 64) {
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));
		}
		else if(IRQNumber >= 64 && IRQNumber < 96) {
			*NVIC_ICER2 |= (1 << (IRQNumber % 64));
		}
	}
}

void USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority) {
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASEADDR + iprx) |= (IRQPriority << shift_amount);
}

void USART_IRQHandling(USART_Handle_t *pUSARTHandle) {
	uint32_t temp1, temp2, temp3;
	uint16_t *pdata;

	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_TC);
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_TCIE);

	if(temp1 && temp2) {
		if(pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
		{
			if(!pUSARTHandle->TxLen) {
				pUSARTHandle->pUSARTx->SR &= ~(1 << USART_SR_TC);
				pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_TCIE);
				pUSARTHandle->TxBusyState = USART_READY;
				pUSARTHandle->pTxBuffer = NULL;
				pUSARTHandle->TxLen = 0;

				USART_ApplicationEventCallback(pUSARTHandle, USART_EV_TX_CMPLT);
			}
		}
	}

	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_TXE);
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_TXEIE);

	if(temp1 && temp2) {
		if(pUSARTHandle->TxBusyState == USART_BUSY_IN_TX) {
			if(pUSARTHandle->TxLen > 0) {
				if(pUSARTHandle->USART_Config.USART_WordLen == USART_WORDLEN_9BIT) {
					pdata = (uint16_t *)pUSARTHandle->pTxBuffer;

					pUSARTHandle->pUSARTx->DR = *pdata & (uint16_t)0x01FF;

					if(pUSARTHandle->USART_Config.USART_ParityCtrl == USART_PARITY_DI) {
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->TxLen -= 2;
					}

					else {
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->TxLen--;
					}
				}

				else {
					pUSARTHandle->pUSARTx->DR = *pUSARTHandle->pTxBuffer & (uint8_t)0xFF;

					pUSARTHandle->pTxBuffer++;
					pUSARTHandle->TxLen--;
				}
			}

			if(pUSARTHandle->TxLen == 0) {
				pUSARTHandle->pUSARTx->CR1 &= ~( 1 << USART_CR1_TXEIE);
			}
		}
	}

	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_RXNE);
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_RXNEIE);

	if(temp1 && temp2) {
		if(pUSARTHandle->RxBusyState == USART_BUSY_IN_RX) {
			if(pUSARTHandle->RxLen > 0) {
				if(pUSARTHandle->USART_Config.USART_WordLen == USART_WORDLEN_9BIT) {
					if(pUSARTHandle->USART_Config.USART_ParityCtrl == USART_PARITY_DI) {
						*((uint16_t *)pUSARTHandle->pRxBuffer) = pUSARTHandle->pUSARTx->DR & (uint16_t)0x01FF;

						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->RxLen -= 2;
					}

					else {
						*pUSARTHandle->pRxBuffer = pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF;

						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->RxLen--;
					}
				}

				else {
					if(pUSARTHandle->USART_Config.USART_ParityCtrl == USART_PARITY_DI) {
						*pUSARTHandle->pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR  & (uint8_t)0xFF);
					}

					else {
						*pUSARTHandle->pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR  & (uint8_t)0x7F);
					}

					pUSARTHandle->pRxBuffer++;
					pUSARTHandle->RxLen--;
				}
			}

			if(!pUSARTHandle->RxLen) {
				pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_RXNEIE);
				pUSARTHandle->RxBusyState = USART_READY;

				USART_ApplicationEventCallback(pUSARTHandle, USART_EV_RX_CMPLT);
			}
		}
	}

	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_CTS);
	temp2 = pUSARTHandle->pUSARTx->CR3 & (1 << USART_CR3_CTSE);
	temp3 = pUSARTHandle->pUSARTx->CR3 & (1 << USART_CR3_CTSIE);

	if(temp1 && temp2 && temp3) {
		pUSARTHandle->pUSARTx->SR &= ~(1 << USART_SR_CTS);

		USART_ApplicationEventCallback(pUSARTHandle, USART_EV_CTS);
	}

	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_IDLE);
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_IDLEIE);

	if(temp1 && temp2) {
		USART_ApplicationEventCallback(pUSARTHandle, USART_EV_IDLE);
	}

	temp1 = pUSARTHandle->pUSARTx->SR & USART_SR_ORE;
	temp2 = pUSARTHandle->pUSARTx->CR1 & USART_CR1_RXNEIE;

	if(temp1 && temp2) {
		USART_ApplicationEventCallback(pUSARTHandle, USART_ER_ORE);
	}

	temp2 = pUSARTHandle->pUSARTx->CR3 & (1 << USART_CR3_EIE);

	if(temp2) {
		temp1 = pUSARTHandle->pUSARTx->SR;

		if(temp1 & (1 << USART_SR_FE)) {
			USART_ApplicationEventCallback(pUSARTHandle, USART_ER_FE);
		}

		if(temp1 & (1 << USART_SR_NF)) {
			USART_ApplicationEventCallback(pUSARTHandle, USART_ER_NE);
		}

		if(temp1 & (1 << USART_SR_ORE)) {
			USART_ApplicationEventCallback(pUSARTHandle, USART_ER_ORE);
		}
	}

}

__weak void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle, uint8_t AppEv) {

}
