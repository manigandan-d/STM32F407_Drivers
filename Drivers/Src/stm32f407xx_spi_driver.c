#include "stm32f407xx_spi_driver.h"

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);

void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		if(pSPIx == SPI1) {
			SPI1_PCLK_EN();
		}

		else if(pSPIx == SPI2) {
			SPI2_PCLK_EN();
		}

		else if(pSPIx == SPI3) {
			SPI3_PCLK_EN();
		}
	}

	else {
		if(pSPIx == SPI1) {
			SPI1_PCLK_DI();
		}

		else if(pSPIx == SPI2) {
			SPI2_PCLK_DI();
		}

		else if(pSPIx == SPI3) {
			SPI3_PCLK_DI();
		}
	}
}

void SPI_Init(SPI_Handle_t *pSPIHandle) {
	uint32_t tempReg = 0;

	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	tempReg = pSPIHandle->SPI_Config.SPI_DeviceMode << SPI_CR1_MSTR;

	if(pSPIHandle->SPI_Config.SPI_BusConfig == SPI_BUS_CONFIG_FD) {
		tempReg &= ~(1 << SPI_CR1_BIDIMODE);
	}

	else if(pSPIHandle->SPI_Config.SPI_BusConfig == SPI_BUS_CONFIG_HD) {
		tempReg |= (1 << SPI_CR1_BIDIMODE);
	}

	else if(pSPIHandle->SPI_Config.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY) {
		tempReg &= ~(1 << SPI_CR1_BIDIMODE);
		tempReg |= (1 << SPI_CR1_RXONLY);
	}

	tempReg |= (pSPIHandle->SPI_Config.SPI_SCLKSpeed << SPI_CR1_BR);

	tempReg |= (pSPIHandle->SPI_Config.SPI_DFF << SPI_CR1_DFF);

	tempReg |= (pSPIHandle->SPI_Config.SPI_CPOL << SPI_CR1_CPOL);

	tempReg |= (pSPIHandle->SPI_Config.SPI_CPHA << SPI_CR1_CPHA);

	pSPIHandle->pSPIx->CR1 = tempReg;
}


void SPI_DeInit(SPI_RegDef_t *pSPIx) {
	if(pSPIx == SPI1) {
		SPI1_REG_RESET();
	}

	else if(pSPIx == SPI2) {
		SPI2_REG_RESET();
	}

	else if(pSPIx == SPI3) {
		SPI3_REG_RESET();
	}
}

uint32_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint8_t flagName) {
	if(pSPIx->SR & flagName) {
		return FLAG_SET;
	}

	return FLAG_RESET;
}

void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len) {
	while(len > 0) {
		while(!SPI_GetFlagStatus(pSPIx, SPI_FLAG_TXE));

		if(pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
			pSPIx->DR = *(uint16_t *)pTxBuffer;
			len -= 2;
			(uint16_t *)pTxBuffer++;
		}

		else {
			pSPIx->DR = *pTxBuffer;
			len--;
			pTxBuffer++;
		}
	}
}

void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t len) {
	while(len > 0) {
		while(!SPI_GetFlagStatus(pSPIx, SPI_FLAG_RXNE));

		if(pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
			*(uint16_t *)pRxBuffer = pSPIx->DR;
			len -= 2;
			(uint16_t *)pRxBuffer++;
		}

		else {
			*pRxBuffer = pSPIx->DR;
			len--;
			pRxBuffer++;
		}
	}
}

void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}

	else {
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}

void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	}

	else {
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}
}

void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}

	else {
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}

void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		if(IRQNumber <= 31) {
			*NVIC_ISER0 |= (1 << IRQNumber);
		}

		else if(IRQNumber > 31 && IRQNumber <= 63) {
			*NVIC_ISER1 |= (1 << IRQNumber % 32);
		}

		else if(IRQNumber > 63 && IRQNumber <= 95) {
			*NVIC_ISER2 |= (1 << IRQNumber % 64);
		}
	}

	else {
		if(IRQNumber <= 31) {
			*NVIC_ICER0 |= (1 << IRQNumber);
		}

		else if(IRQNumber > 31 && IRQNumber <= 63) {
			*NVIC_ICER1 |= (1 << IRQNumber % 32);
		}

		else if(IRQNumber > 63 && IRQNumber <= 95) {
			*NVIC_ICER2 |= (1 << IRQNumber % 64);
		}
	}
}

void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority) {
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASEADDR + iprx) |= (IRQPriority << shift_amount);
}

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t len) {
	uint8_t state = pSPIHandle->TxState;

	if(state != SPI_BUSY_IN_TX) {
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = len;

		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
	}

	return state;
}

uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t len) {
	uint8_t state = pSPIHandle->RxState;

	if(state != SPI_BUSY_IN_RX) {
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = len;

		pSPIHandle->RxState = SPI_BUSY_IN_RX;

		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
	}

	return state;
}

void SPI_IRQHandling(SPI_Handle_t *pSPIHandle) {
	uint8_t temp1, temp2;

	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	if(temp1 && temp2) {
		spi_txe_interrupt_handle(pSPIHandle);
	}

	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

	if(temp1 && temp2) {
		spi_rxne_interrupt_handle(pSPIHandle);
	}

	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_OVR);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if(temp1 && temp2) {
		spi_ovr_err_interrupt_handle(pSPIHandle);
	}
}

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle) {
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
		pSPIHandle->pSPIx->DR = *((uint16_t *)pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen -= 2;
		(uint16_t *)pSPIHandle->pTxBuffer++;
	}

	else {
		pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer++;
	}

	if(!pSPIHandle->TxLen) {
		SPI_CloseTransmission(pSPIHandle);

		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
	}
}

static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle) {
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
		*((uint16_t *)pSPIHandle->pRxBuffer) = (uint16_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen -= 2;
		(uint16_t *)pSPIHandle->pRxBuffer++;
	}

	else {
		*pSPIHandle->pRxBuffer = pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->pRxBuffer++;
	}

	if(!pSPIHandle->RxLen) {
		SPI_CloseReception(pSPIHandle);

		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
	}
}

static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle) {
	uint8_t temp;

	if(pSPIHandle->TxState != SPI_BUSY_IN_TX) {
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
	}

	(void)temp;

	SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);
}

void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle) {
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;
}

void SPI_CloseReception(SPI_Handle_t *pSPIHandle) {
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
}

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx) {
	uint8_t temp;

	temp = pSPIx->DR;
	temp = pSPIx->SR;

	(void)temp;
}

__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv) {

}
