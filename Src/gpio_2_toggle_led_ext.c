/*
 * gpio_2_toggle_led_ext.c
 *
 *  Created on: Jan 20, 2024
 *      Author: MANIGANDAN D
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "stm32f407xx.h"

#define HIGH 1
#define LOW 0
#define BTN_PRESSED LOW
//#define BTN_PRESSED LOW - for nucleo & external push button (pull up configuration)

void delay() {
	for(int i=0; i<200000; i++);
}

void GPIO_ButtonInit(void) {

	GPIO_Handle_t GPIOBtn;

	memset(&GPIOBtn, 0, sizeof(GPIOBtn));

	GPIOBtn.pGPIOx = GPIOB;

	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU;

	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&GPIOBtn);
}

void GPIO_LedInit(void) {

	GPIO_Handle_t GPIOLed;

	memset(&GPIOLed, 0, sizeof(GPIOLed));

	GPIOLed.pGPIOx = GPIOA;

	GPIOLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIOLed.GPIO_PinConfig.GPIO_PinOType = GPIO_OP_TYPE_PP;
	GPIOLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIOLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIG;

	GPIOLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GPIO_Init(&GPIOLed);
}

int main(void) {
	GPIO_ButtonInit();
	GPIO_LedInit();

	while(1) {
		if(GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_12) == BTN_PRESSED) {
			delay();

			GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_8);
		}
	}
}
