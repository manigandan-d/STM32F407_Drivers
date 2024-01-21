/*
 * i2c_2_master_rx.c
 *
 *  Created on: Jan 20, 2024
 *      Author: MANIGANDAN D
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "stm32f407xx.h"

#define MY_ADDR 0x61
#define SLAVE_ADDR 0x68

I2C_Handle_t I2CHandle;

uint8_t data[32];

void delay() {
	for(int i=0; i<200000; i++);
}

void I2C1_GPIOInit(void) {
	GPIO_Handle_t I2CPins;
	memset(&I2CPins, 0, sizeof(I2CPins));

	I2CPins.pGPIOx = GPIOB;

	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIG;
	I2CPins.GPIO_PinConfig.GPIO_PinOType = GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&I2CPins);

	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&I2CPins);
}

void I2C1_Init(void) {
	I2CHandle.pI2Cx = I2C1;

	I2CHandle.I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
	I2CHandle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2CHandle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
	I2CHandle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;

	I2C_Init(&I2CHandle);
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
	uint8_t commandCode=0, len=0;

	printf("Application is running\n");

	GPIO_ButtonInit();

	I2C1_GPIOInit();

	I2C1_Init();

	I2C_PeripheralControl(I2C1, ENABLE);

	I2C_ManageACK(I2C1, I2C_ACK_ENABLE);

	while(1) {
		while(!(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)));
		delay();

		commandCode = 0x51;

		I2C_MasterSendData(&I2CHandle, &commandCode, 1, SLAVE_ADDR, I2C_ENABLE_SR);
		I2C_MasterReceiveData(&I2CHandle, &len, 1, SLAVE_ADDR, I2C_ENABLE_SR);

		commandCode = 0x52;

		I2C_MasterSendData(&I2CHandle, &commandCode, 1, SLAVE_ADDR, I2C_ENABLE_SR);
		I2C_MasterReceiveData(&I2CHandle, data, len, SLAVE_ADDR, I2C_DISABLE_SR);

		data[len+1] = '\0';

		printf("Data: %s", data);
	}
}
