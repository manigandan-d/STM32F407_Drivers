/*
 * usart_1_tx.c
 *
 *  Created on: Jan 20, 2024
 *      Author: MANIGANDAN D
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "stm32f407xx.h"

USART_Handle_t USART2Handle;

char data[] = "hello from usart";

void delay() {
	for(int i=0; i<200000; i++);
}

void USART2_GPIOInit(void) {
	GPIO_Handle_t USARTPins;
	memset(&USARTPins, 0, sizeof(USARTPins));

	USARTPins.pGPIOx = GPIOA;

	USARTPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	USARTPins.GPIO_PinConfig.GPIO_PinAltFunMode = 7;
	USARTPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIG;
	USARTPins.GPIO_PinConfig.GPIO_PinOType = GPIO_OP_TYPE_PP;
	USARTPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	USARTPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
	GPIO_Init(&USARTPins);

	USARTPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIO_Init(&USARTPins);
}

void USART2_Init(void) {
	USART2Handle.pUSARTx = USART2;

	USART2Handle.USART_Config.USART_Mode = USART_MODE_TX_ONLY;
	USART2Handle.USART_Config.USART_Baud = USART_BAUD_115200;
	USART2Handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	USART2Handle.USART_Config.USART_WordLen = USART_WORDLEN_8BIT;
	USART2Handle.USART_Config.USART_ParityCtrl = USART_PARITY_DI;
	USART2Handle.USART_Config.USART_HWFlowCtrl = USART_HWFLOWCTRL_NONE;

	USART_Init(&USART2Handle);
}

void GPIO_ButtonInit(void) {

	GPIO_Handle_t GPIOBtn;
	memset(&GPIOBtn, 0, sizeof(GPIOBtn));

	GPIOBtn.pGPIOx = GPIOA;

	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIO_Init(&GPIOBtn);
}

int main(void) {

	GPIO_ButtonInit();

	USART2_GPIOInit();

	USART2_Init();

	USART_PeripheralControl(USART2, ENABLE);

	while(1) {
		while(!(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)));
		delay();

		USART_SendData(&USART2Handle, (uint8_t *)data, strlen(data));
	}
}
