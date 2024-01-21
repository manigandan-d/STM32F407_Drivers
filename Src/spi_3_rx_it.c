/*
 * spi_3_rx_it.c
 *
 *  Created on: Jan 20, 2024
 *      Author: MANIGANDAN D
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "stm32f407xx.h"

#define MAX 500

SPI_Handle_t SPIHandle;

volatile uint8_t dataAvailable = 0;
volatile uint8_t rcvStop = 0;

char RcvBuffer[MAX];

volatile char ReadByte;

void delay() {
	for(uint32_t i=0; i<200000; i++);
}

void SPI2_GPIOInit(void) {
	GPIO_Handle_t SPIPins;
	memset(&SPIPins, 0, sizeof(SPIPins));

	SPIPins.pGPIOx = GPIOB;

	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIG;

	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);

	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPIPins);

	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPIPins);
}

void SPI2_Init(void) {
	SPIHandle.pSPIx = SPI2;

	SPIHandle.SPI_Config.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPIHandle.SPI_Config.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPIHandle.SPI_Config.SPI_SCLKSpeed = SPI_SCLK_SPEED_DIV8; //32
	SPIHandle.SPI_Config.SPI_DFF = SPI_DFF_8BIT;
	SPIHandle.SPI_Config.SPI_CPOL = SPI_CPOL_LOW;
	SPIHandle.SPI_Config.SPI_CPHA = SPI_CPHA_LOW;
	SPIHandle.SPI_Config.SPI_SSM = SPI_SSM_DI;

	SPI_Init(&SPIHandle);
}

void Slave_GPIO_InterruptPinInit(void) {
	GPIO_Handle_t spiIntPin;
	memset(&spiIntPin, 0, sizeof(spiIntPin));

	spiIntPin.pGPIOx = GPIOD;

	spiIntPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	spiIntPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU;

	spiIntPin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&spiIntPin);

	GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, ENABLE);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, NVIC_IRQ_PRI15);
}

int main(void) {
	uint8_t dummy = 0xFF;

	Slave_GPIO_InterruptPinInit();

	SPI2_GPIOInit();

	SPI2_Init();

	SPI_SSOEConfig(SPI2, ENABLE);

	SPI_IRQInterruptConfig(IRQ_NO_SPI2, ENABLE);
	SPI_IRQPriorityConfig(IRQ_NO_SPI2, NVIC_IRQ_PRI15);

	while(1) {
		rcvStop = 0;

		while(!dataAvailable);

		GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, DISABLE);

		SPI_PeripheralControl(SPI2, ENABLE);

		while(!rcvStop) {
			while(SPI_SendDataIT(&SPIHandle, &dummy, 1) == SPI_BUSY_IN_TX);
			while(SPI_ReceiveDataIT(&SPIHandle, &ReadByte, 1) == SPI_BUSY_IN_RX);
		}

		while(SPI_GetFlagStatus(SPI2, SPI_FLAG_BSY));
		SPI_PeripheralControl(SPI2, DISABLE);

		printf("Received Data: %s\n", RcvBuffer);

		dataAvailable = 0;

		GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, ENABLE);
	}

	return 0;
}

void SPI2_IRQHandler(void) {
	SPI_IRQHandling(&SPIHandle);
}

void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv) {
	static uint32_t i = 0;

	if(AppEv == SPI_EVENT_RX_CMPLT) {
		RcvBuffer[i++] = ReadByte;

		if(ReadByte == '\0' || (i == MAX)) {
			rcvStop = 1;
			RcvBuffer[i-1] = '\0';
			i=0;
		}
	}
}

void EXTI9_5_IRQHandler(void) {
	GPIO_IRQHandling(GPIO_PIN_NO_6);

	dataAvailable = 1;
}
