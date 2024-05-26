#include "stm32f407xx_gpio_driver.h"

void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		if(pGPIOx == GPIOA) {
			GPIOA_PCLK_EN();
		}

		else if(pGPIOx == GPIOB) {
			GPIOB_PCLK_EN();
		}

		else if(pGPIOx == GPIOC) {
			GPIOC_PCLK_EN();
		}

		else if(pGPIOx == GPIOD) {
			GPIOD_PCLK_EN();
		}

		else if(pGPIOx == GPIOE) {
			GPIOE_PCLK_EN();
		}

		else if(pGPIOx == GPIOF) {
			GPIOF_PCLK_EN();
		}

		else if(pGPIOx == GPIOG) {
			GPIOG_PCLK_EN();
		}

		else if(pGPIOx == GPIOH) {
			GPIOH_PCLK_EN();
		}

		else if(pGPIOx == GPIOI) {
			GPIOI_PCLK_EN();
		}

	}

	else {
		if(pGPIOx == GPIOA) {
			GPIOA_PCLK_DI();
		}

		else if(pGPIOx == GPIOB) {
			GPIOB_PCLK_DI();
		}

		else if(pGPIOx == GPIOC) {
			GPIOC_PCLK_DI();
		}

		else if(pGPIOx == GPIOD) {
			GPIOD_PCLK_DI();
		}

		else if(pGPIOx == GPIOE) {
			GPIOE_PCLK_DI();
		}

		else if(pGPIOx == GPIOF) {
			GPIOF_PCLK_DI();
		}

		else if(pGPIOx == GPIOG) {
			GPIOG_PCLK_DI();
		}

		else if(pGPIOx == GPIOH) {
			GPIOH_PCLK_DI();
		}

		else if(pGPIOx == GPIOI) {
			GPIOI_PCLK_DI();
		}

	}
}

void GPIO_Init(GPIO_Handle_t *pGPIOHandle) {
	uint32_t temp = 0;

	GPIO_PeriClockControl(pGPIOHandle->pGPIOx, ENABLE);

	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG) {
		temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->MODER |= temp;
	}

	else {
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT) {
				EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
				EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}

		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT) {
				EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
				EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}

		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT) {
				EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
				EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}

		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4;
		uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;

		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);

		SYSCFG_PCLK_EN();

		SYSCFG->EXTICR[temp1] = portcode << (temp2 * 4);

		EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}

	temp = 0;

	temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;

	temp = 0;

	temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR |= temp;

	temp = 0;

	temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinOType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN) {
		uint8_t temp1, temp2;

		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;

		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF << (4 * temp2));
		pGPIOHandle->pGPIOx->AFR[temp1] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2));
	}
}

void GPIO_DeInit(GPIO_RegDef_t *pGPIOx) {
	if(pGPIOx == GPIOA) {
		GPIOA_REG_RESET();
	}

	else if(pGPIOx == GPIOB) {
		GPIOB_REG_RESET();
	}

	else if(pGPIOx == GPIOC) {
		GPIOC_REG_RESET();
	}

	else if(pGPIOx == GPIOD) {
		GPIOD_REG_RESET();
	}

	else if(pGPIOx == GPIOE) {
		GPIOE_REG_RESET();
	}

	else if(pGPIOx == GPIOF) {
		GPIOF_REG_RESET();
	}

	else if(pGPIOx == GPIOG) {
		GPIOG_REG_RESET();
	}

	else if(pGPIOx == GPIOH) {
		GPIOH_REG_RESET();
	}

	else if(pGPIOx == GPIOI) {
		GPIOI_REG_RESET();
	}

}

uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber) {
	uint8_t data;

	data = (uint8_t)(pGPIOx->IDR >> PinNumber) & 0x00000001;

	return data;
}

uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx) {
	uint16_t data;

	data = (uint16_t)(pGPIOx->IDR);

	return data;
}

void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t value) {
	if(value == GPIO_PIN_SET) {
		pGPIOx->ODR |= (1<<PinNumber);
	}

	else {
		pGPIOx->ODR &= ~(1<<PinNumber);
	}

}

void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t value) {
	pGPIOx->ODR = value;
}

void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber) {
	pGPIOx->ODR ^= (1<<PinNumber);
}

void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi) {
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

void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority) {
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASEADDR + iprx) |= (IRQPriority << shift_amount);
}

void GPIO_IRQHandling(uint8_t PinNumber) {
	if(EXTI->PR & (1 << PinNumber)) {
		EXTI->PR |= (1 << PinNumber);
	}
}
