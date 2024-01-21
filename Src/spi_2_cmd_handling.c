/*
 * spi_2_cmd_handling.c
 *
 *  Created on: Jan 21, 2024
 *      Author: MANIGANDAN D
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "stm32f407xx.h"

#define CMD_LED_CTRL 0x50
#define CMD_SENSOR_READ 0x51
#define CMD_LED_READ 0x52
#define CMD_PRINT 0x53
#define CMD_ID_READ 0x54

#define LED_OFF 0
#define LED_ON 1

#define LED_PIN 9

#define ANALOG_PIN_NO_0 0
#define ANALOG_PIN_NO_1 1
#define ANALOG_PIN_NO_2 2
#define ANALOG_PIN_NO_3 3
#define ANALOG_PIN_NO_4 4

SPI_Handle_t SPIHandle;

void delay() {
	for(int i=0; i<200000; i++);
}

void SPI2_GPIOInit(void) {
	GPIO_Handle_t SPIPins;
	memset(&SPIPins, 0, sizeof(SPIPins));

	SPIPins.pGPIOx = GPIOB;

	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIG;
	SPIPins.GPIO_PinConfig.GPIO_PinOType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

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
	SPIHandle.SPI_Config.SPI_SCLKSpeed = SPI_SCLK_SPEED_DIV8; // 32
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

uint8_t SPI_VerifyResponse(uint8_t ackByte) {
	if(ackByte == (uint8_t)0xF5)
		return 1;

	return 0;
}

int main(void) {
	uint8_t dummyRead;
	uint8_t dummyWrite = 0xFF;

	printf("Application is running...\n");

	GPIO_ButtonInit();

	SPI2_GPIOInit();

	SPI2_Init();

	printf("SPI Initialization done!\n");

	SPI_SSOEConfig(SPI2, ENABLE);

	while(1) {
		while(!(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)));
		delay();

		SPI_PeripheralControl(SPI2, ENABLE);

		uint8_t cmdCode = CMD_LED_CTRL;
		uint8_t ackByte;
		uint8_t args[2];

		SPI_SendData(SPI2, &cmdCode, 1);
		SPI_ReceiveData(SPI2, &dummyRead, 1);

		SPI_SendData(SPI2, &dummyWrite, 1);
		SPI_ReceiveData(SPI2, &ackByte, 1);

		if(SPI_VerifyResponse(ackByte)) {
			args[0] = LED_PIN;
			args[1] = LED_ON;

			SPI_SendData(SPI2, args, 2);
			SPI_ReceiveData(SPI2, args, 2);

			printf("Led control command executed successfully!\n");
		}

		while(!(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)));
		delay();

		cmdCode = CMD_SENSOR_READ;

		SPI_SendData(SPI2, &cmdCode, 1);
		SPI_ReceiveData(SPI2, &dummyRead, 1);

		SPI_SendData(SPI2, &dummyWrite, 1);
		SPI_ReceiveData(SPI2, &ackByte, 1);

		if(SPI_VerifyResponse(ackByte)) {
			args[0] = ANALOG_PIN_NO_0;

			SPI_SendData(SPI2, args, 1);
			SPI_ReceiveData(SPI2, &dummyRead, 1);

			delay();

			SPI_SendData(SPI2, &dummyWrite, 1);

			uint8_t data;

			SPI_ReceiveData(SPI2, &data, 1);

			printf("Sensor value: %d\n", data);
			printf("Sensor read command executed successfully\n");
		}

		while(!(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)));
		delay();

		cmdCode = CMD_LED_READ;

		SPI_SendData(SPI2, &cmdCode, 1);
		SPI_ReceiveData(SPI2, &dummyRead, 1);

		SPI_SendData(SPI2, &dummyWrite, 1);
		SPI_ReceiveData(SPI2, &ackByte, 1);

		if(SPI_VerifyResponse(ackByte)) {
			args[0] = LED_PIN;

			SPI_SendData(SPI2, args, 1);
			SPI_ReceiveData(SPI2, &dummyRead, 1);

			delay();

			SPI_SendData(SPI2, &dummyWrite, 1);

			uint8_t status;

			SPI_ReceiveData(SPI2, &status, 1);

			printf("LED Status: %d\n", status);
			printf("LED status command executed successfully\n");
		}

		while(!(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)));
		delay();

		cmdCode = CMD_PRINT;

		SPI_SendData(SPI2, &cmdCode, 1);
		SPI_ReceiveData(SPI2, &dummyRead, 1);

		SPI_SendData(SPI2, &dummyWrite, 1);
		SPI_ReceiveData(SPI2, &ackByte, 1);

		uint8_t msg[] = "Hello from STM";

		if(SPI_VerifyResponse(ackByte)) {
			args[0] = strlen((char *)msg);

			SPI_SendData(SPI2, args, 1);
			SPI_ReceiveData(SPI2, &dummyRead, 1);

			delay();

			for(uint32_t i=0 ; i<args[0]; i++) {
				SPI_SendData(SPI2, msg+i, 1);
				SPI_ReceiveData(SPI2, &dummyRead, 1);
			}

			printf("Message send command executed successfully\n");
		}

		while(!(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)));
		delay();

		cmdCode = CMD_ID_READ;

		SPI_SendData(SPI2, &cmdCode, 1);
		SPI_ReceiveData(SPI2, &dummyRead, 1);

		SPI_SendData(SPI2, &dummyWrite, 1);
		SPI_ReceiveData(SPI2, &ackByte, 1);

		uint8_t id[11];

		if(SPI_VerifyResponse(ackByte)) {
			for(uint32_t i=0; i<10; i++) {
				SPI_SendData(SPI2, &dummyWrite, 1);
				SPI_ReceiveData(SPI2, id+1, 1);
			}

			id[10] = '\0';

			printf("Get ID command executed successfully\n");
 		}

		while(SPI_GetFlagStatus(SPI2, SPI_FLAG_BSY));
		SPI_PeripheralControl(SPI2, DISABLE);

		printf("SPI communication closed!\n");
	}
}
