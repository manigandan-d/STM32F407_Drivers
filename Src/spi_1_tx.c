/*
 * spi_1_tx.c
 *
 *  Created on: Jan 20, 2024
 *      Author: MANIGANDAN D
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "stm32f407xx.h"

SPI_Handle_t SPIHandle;

char data[] = "hello from spi";

void delay() {
	for(int i=0; i<200000; i++);
}

void SPI2_GPIOInit(void) {
	GPIO_Handle_t SPIPins;
	memset(&SPIPins, 0, sizeof(SPIPins));

	SPIPins.pGPIOx = GPIOB;

	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_VHIG;
	SPIPins.GPIO_PinConfig.GPIO_PinOType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;

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
	SPIHandle.SPI_Config.SPI_SCLKSpeed = SPI_SCLK_SPEED_DIV8;
	SPIHandle.SPI_Config.SPI_DFF = SPI_DFF_8BIT;
	SPIHandle.SPI_Config.SPI_CPOL = SPI_CPOL_LOW;
	SPIHandle.SPI_Config.SPI_CPHA = SPI_CPHA_LOW;
	SPIHandle.SPI_Config.SPI_SSM = SPI_SSM_DI;

	SPI_Init(&SPIHandle);
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

	SPI2_GPIOInit();

	SPI2_Init();

	SPI_SSOEConfig(SPI2, ENABLE);

	while(1) {
		while(!(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)));
		delay();

		SPI_PeripheralControl(SPI2, ENABLE);

		uint8_t dataLen = strlen(data);

		SPI_SendData(SPI2, &dataLen, 1);

		SPI_SendData(SPI2, (uint8_t *)data, strlen(data));

		while(SPI_GetFlagStatus(SPI2, SPI_FLAG_BSY));

		SPI_PeripheralControl(SPI2, DISABLE);
	}
}
