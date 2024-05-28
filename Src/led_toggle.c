#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "stm32f407xx.h"

void delay() {
	for(int i=0; i<200000; i++);
}

void GPIO_LedInit(void) {
	GPIO_Handle_t GPIOLed;
	memset(&GPIOLed, 0, sizeof(GPIOLed));

	GPIOLed.pGPIOx = GPIOD;

	GPIOLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIOLed.GPIO_PinConfig.GPIO_PinOType = GPIO_OP_TYPE_PP;
	GPIOLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIOLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIG;

	GPIOLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&GPIOLed);
}

int main(void) {
	GPIO_ButtonInit();
	GPIO_LedInit();

	while(1) {
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
		delay();
	}
}
