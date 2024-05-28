#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "stm32f407xx.h"

GPIO_Handle_t GPIORow;
GPIO_Handle_t GPIOCol;

void delay() {
	for(int i=0; i<200000; i++);
}

void GPIO_RowInit(void) {
	GPIORow.pGPIOx = GPIOD;

	GPIORow.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIORow.GPIO_PinConfig.GPIO_PinOType = GPIO_OP_TYPE_PP;
	GPIORow.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIORow.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIG;

	GPIORow.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIO_Init(&GPIORow);

	GPIORow.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
	GPIO_Init(&GPIORow);

	GPIORow.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
	GPIO_Init(&GPIORow);

	GPIORow.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIO_Init(&GPIORow);
}

void GPIO_ColInit(void) {
	GPIOCol.pGPIOx = GPIOD;

	GPIOCol.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOCol.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU;

	GPIOCol.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GPIO_Init(&GPIOCol);

	GPIOCol.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
	GPIO_Init(&GPIOCol);

	GPIOCol.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
	GPIO_Init(&GPIOCol);

	GPIOCol.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_11;
	GPIO_Init(&GPIOCol);
}

int main(void) {
	GPIO_ColInit();
	GPIO_RowInit();

	printf("Application is running...\n");

	while(1) {
		GPIORow.pGPIOx->ODR |= 0x0F;
		GPIORow.pGPIOx->ODR &= ~(1 << 0);

		if(!(GPIOCol.pGPIOx->IDR & (1 << 8))) {
			delay();
			printf("1\n");
		}

		if(!(GPIOCol.pGPIOx->IDR & (1 << 9))) {
			delay();
			printf("2\n");
		}

		if(!(GPIOCol.pGPIOx->IDR & (1 << 10))) {
			delay();
			printf("3\n");
		}

		if(!(GPIOCol.pGPIOx->IDR & (1 << 11))) {
			delay();
			printf("A\n");
		}

		GPIORow.pGPIOx->ODR |= 0x0F;
		GPIORow.pGPIOx->ODR &= ~(1 << 1);

		if(!(GPIOCol.pGPIOx->IDR & (1 << 8))) {
			delay();
			printf("4\n");
		}

		if(!(GPIOCol.pGPIOx->IDR & (1 << 9))) {
			delay();
			printf("5\n");
		}

		if(!(GPIOCol.pGPIOx->IDR & (1 << 10))) {
			delay();
			printf("6\n");
		}

		if(!(GPIOCol.pGPIOx->IDR & (1 << 11))) {
			delay();
			printf("B\n");
		}

		GPIORow.pGPIOx->ODR |= 0x0F;
		GPIORow.pGPIOx->ODR &= ~(1 << 2);

		if(!(GPIOCol.pGPIOx->IDR & (1 << 8))) {
			delay();
			printf("7\n");
		}

		if(!(GPIOCol.pGPIOx->IDR & (1 << 9))) {
			delay();
			printf("8\n");
		}

		if(!(GPIOCol.pGPIOx->IDR & (1 << 10))) {
			delay();
			printf("9\n");
		}

		if(!(GPIOCol.pGPIOx->IDR & (1 << 11))) {
			delay();
			printf("C\n");
		}

		GPIORow.pGPIOx->ODR |= 0x0F;
		GPIORow.pGPIOx->ODR &= ~(1 << 3);

		if(!(GPIOCol.pGPIOx->IDR & (1 << 8))) {
			delay();
			printf("*\n");
		}

		if(!(GPIOCol.pGPIOx->IDR & (1 << 9))) {
			delay();
			printf("0\n");
		}

		if(!(GPIOCol.pGPIOx->IDR & (1 << 10))) {
			delay();
			printf("#\n");
		}

		if(!(GPIOCol.pGPIOx->IDR & (1 << 11))) {
			delay();
			printf("D\n");
		}
	}
}
