#include "stm32f407xx_i2c_driver.h"

void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	}

	else {
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
	}
}

void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		if(pI2Cx == I2C1) {
			I2C1_PCLK_EN();
		}

		else if(pI2Cx == I2C2) {
			I2C2_PCLK_EN();
		}

		else if(pI2Cx == I2C3) {
			I2C3_PCLK_EN();
		}

	}

	else {
		if(pI2Cx == I2C1) {
			I2C1_PCLK_DI();
		}

		else if(pI2Cx == I2C2) {
			I2C2_PCLK_DI();
		}

		else if(pI2Cx == I2C3) {
			I2C3_PCLK_DI();
		}
	}
}

void I2C_Init(I2C_Handle_t *pI2CHandle) {
	uint32_t temp = 0; // it can be 16 bit

	I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

	temp = pI2CHandle->I2C_Config.I2C_AckControl << I2C_CR1_ACK;

	pI2CHandle->pI2Cx->CR1 = temp;

	temp = RCC_GetPCLK1Value() / 1000000U;

	pI2CHandle->pI2Cx->CR2 = temp; // pI2CHandle->pI2Cx->CR2 = temp & 0x3F;

	temp = pI2CHandle->I2C_Config.I2C_DeviceAddress << 1;
	temp |= (1 << 14);

	pI2CHandle->pI2Cx->OAR1 = temp;

	uint16_t ccr = 0;

	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM) {
		ccr = RCC_GetPCLK1Value() / (2 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		temp = ccr & 0xFFF;
	}

	else {
		temp = (1 << 15);
		temp |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << 14);

		if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2) {
			ccr = RCC_GetPCLK1Value() / (3 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		}

		else {
			ccr = RCC_GetPCLK1Value() / (25 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		}

		temp |= ccr & 0xFFF;
	}

	pI2CHandle->pI2Cx->CCR = temp;

	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM) {
		temp = (RCC_GetPCLK1Value() / 1000000U) + 1;
	}

	else {
		temp = (RCC_GetPCLK1Value() / 300000U) + 1;
	}

	pI2CHandle->pI2Cx->TRISE = temp & 0x3F;

}

void I2C_DeInit(I2C_RegDef_t *pI2Cx) {
	if(pI2Cx == I2C1) {
		I2C1_REG_RESET();
	}

	else if(pI2Cx == I2C2) {
		I2C2_REG_RESET();
	}

	else if(pI2Cx == I2C3) {
		I2C3_REG_RESET();
	}
}

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx) {
	pI2Cx->CR1 |= (1 << I2C_CR1_START);
}

uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t flagName) {
	if(pI2Cx->SR1 & flagName) {
		return FLAG_SET;
	}

	return FLAG_RESET;
}

static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t slaveAddr) {
	slaveAddr = slaveAddr << 1;
	slaveAddr &= ~(1);

	pI2Cx->DR = slaveAddr;
}

static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t slaveAddr) {
	slaveAddr = slaveAddr << 1;
	slaveAddr |= 1;

	pI2Cx->DR = slaveAddr;
}

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle) {
	uint32_t dummyRead = 0;

	if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL)) {
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX) {
			if(pI2CHandle->RxSize == 1) {
				I2C_ManageACK(pI2CHandle->pI2Cx, DISABLE);

				dummyRead = pI2CHandle->pI2Cx->SR1;
				dummyRead = pI2CHandle->pI2Cx->SR2;

				(void)dummyRead;
			}
		}

		else {
			dummyRead = pI2CHandle->pI2Cx->SR1;
			dummyRead = pI2CHandle->pI2Cx->SR2;

			(void)dummyRead;
		}
	}

	else {
		dummyRead = pI2CHandle->pI2Cx->SR1;
		dummyRead = pI2CHandle->pI2Cx->SR2;

		(void)dummyRead;
	}
}

void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx) {
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}

void I2C_SlaveEnDiCallbackEvents(I2C_RegDef_t *pI2Cx, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);
		pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
		pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);
	}

	else {
		pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);
		pI2Cx->CR2 &= ~(1 << I2C_CR2_ITERREN);
		pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);
	}
}

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint8_t len, uint8_t slaveAddr, uint8_t Sr) {
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB)));

	I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, slaveAddr);

	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR)));

	I2C_ClearADDRFlag(pI2CHandle);

	while(len > 0) {
		while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE)));

		pI2CHandle->pI2Cx->DR = *pTxBuffer;
		pTxBuffer++;
		len--;
	}

	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE)));

	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF)));

	if(Sr == I2C_DISABLE_SR) {
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
	}
}

void I2C_ManageACK(I2C_RegDef_t *pI2Cx, uint8_t EnorDi) {
	if(EnorDi == I2C_ACK_ENABLE) {
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
	}

	else {
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
	}
}

void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint8_t len, uint8_t slaveAddr, uint8_t Sr) {
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB)));

	I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, slaveAddr);

	I2C_ClearADDRFlag(pI2CHandle);

	if(len == 1) {
		I2C_ManageACK(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

		I2C_ClearADDRFlag(pI2CHandle);

		while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE)));

		if(Sr == I2C_DISABLE_SR) {
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}

		*pRxBuffer = pI2CHandle->pI2Cx->DR;
	}

	if(len > 1) {
		I2C_ClearADDRFlag(pI2CHandle);

		for(uint32_t i=len; i>0; i--) {
			while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE)));

			if(i == 2) {
				I2C_ManageACK(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

				if(Sr == I2C_DISABLE_SR) {
					I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
				}
			}

			*pRxBuffer = pI2CHandle->pI2Cx->DR;

			pRxBuffer++;
		}
	}

	if(pI2CHandle->I2C_Config.I2C_AckControl == ENABLE) {
		I2C_ManageACK(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);
	}
}

uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint8_t len, uint8_t slaveAddr, uint8_t Sr) {
	uint8_t busystate = pI2CHandle->TxRxState;

	if((busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX)) {
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = slaveAddr;
		pI2CHandle->Sr = Sr;

		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);

		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
	}

	return busystate;
}

uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint8_t len, uint8_t slaveAddr, uint8_t Sr) {
	uint8_t busystate = pI2CHandle->TxRxState;

	if((busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX)) {
		pI2CHandle->pRxBuffer = pRxBuffer;
		pI2CHandle->RxLen = len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = len;
		pI2CHandle->DevAddr = slaveAddr;
		pI2CHandle->Sr = Sr;

		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);

		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
	}

	return busystate;
}

void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx, uint8_t data) {
	pI2Cx->DR = data;
}

uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx) {
	return (uint8_t)pI2Cx->DR;
}

void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi) {
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

void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority) {
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASEADDR + iprx) |= (IRQPriority << shift_amount);
}

static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle) {
	if(pI2CHandle->TxLen > 0) {
		pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);
		pI2CHandle->pTxBuffer++;
		pI2CHandle->TxLen--;
	}
}

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle) {
	if(pI2CHandle->RxSize == 1) {
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxSize > 1) {
		if(pI2CHandle->RxLen == 2) {
			I2C_ManageACK(pI2CHandle->pI2Cx, DISABLE);
		}

		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->pRxBuffer++;
		pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxLen == 0) {
		if(pI2CHandle->Sr == I2C_DISABLE_SR) {
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

			I2C_CloseReception(pI2CHandle);

			I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_CMPLT);
		}
	}
}

void I2C_CloseTransmission(I2C_Handle_t *pI2CHandle) {
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->TxLen = 0;
}

void I2C_CloseReception(I2C_Handle_t *pI2CHandle) {
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxBuffer = NULL;
	pI2CHandle->RxLen = 0;
	pI2CHandle->RxSize = 0;

	if(pI2CHandle->I2C_Config.I2C_AckControl == I2C_ACK_ENABLE) {
		I2C_ManageACK(pI2CHandle->pI2Cx,ENABLE);
	}
}

void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle) {
	uint32_t temp1, temp2, temp3;

	temp1 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITEVTEN);
	temp2 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN);

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_SB);

	if(temp1 && temp3) {
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX) {
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}

		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX){
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADDR);

	if(temp1 && temp3) {
		I2C_ClearADDRFlag(pI2CHandle);
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_BTF);

	if(temp1 && temp3) {
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX) {
			if(pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE)) {
				if(pI2CHandle->TxLen == 0) {
					if(pI2CHandle->Sr == I2C_DISABLE_SR) {
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
					}

					I2C_CloseTransmission(pI2CHandle);

					I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
				}
			}
		}

		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX) {
			;
		}
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_STOPF);

	if(temp1 && temp3) {
		pI2CHandle->pI2Cx->CR2 |= 0x0000;

		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP);
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE);

	if(temp1 && temp2 && temp3) {
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL)) {
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX) {
				I2C_MasterHandleTXEInterrupt(pI2CHandle);
			}
		}

		else {
			if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA)) {
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_REQ);
			}
		}
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RXNE);

	if(temp1 && temp2 && temp3) {
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL)) {
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX) {
				I2C_MasterHandleRXNEInterrupt(pI2CHandle);
			}
		}

		else {
			if(!(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA))) {
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_RCV);
			}
		}
	}
}

void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle) {
	uint32_t temp1, temp2;

	temp2 = (pI2CHandle->pI2Cx->CR2) & (1 << I2C_CR2_ITERREN);

	temp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_BERR);

	if(temp1 && temp2) {
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_BERR);

		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_BERR);
	}

	temp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_ARLO);

	if(temp1 && temp2) {
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_ARLO);

		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_ARLO);
	}

	temp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_AF);

	if(temp1 && temp2) {
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_AF);

		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_AF);
	}

	temp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_OVR);

	if(temp1 && temp2) {
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_OVR);

		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_OVR);
	}

	temp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_TIMEOUT);

	if(temp1 && temp2) {
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_TIMEOUT);

		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_TIMEOUT);
	}
}
