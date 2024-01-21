/*
 * stm32f407xx_rcc_driver.c
 *
 *  Created on: Jan 20, 2024
 *      Author: MANIGANDAN D
 */

#include "stm32f407xx_rcc_driver.h"

uint16_t AHB_PreScaler[8] = {2, 4, 8, 16, 64, 128, 256, 512};
uint16_t APB_PreScaler[4] = {2, 4, 8, 16};

uint32_t RCC_GetPLLOutputClock() {
	return 0;
}

uint32_t RCC_GetPCLK1Value(void) {
	uint32_t pClk1, systemClk;

	uint8_t clkSrc, temp, ahbp, apb1p;

	clkSrc = (RCC->CFGR >> 2) & 0x3;

	if(clkSrc == 0) {
		systemClk =  16000000;
	}

	else if(clkSrc == 1) {
		systemClk = 8000000;
	}

	else if(clkSrc == 2) {
		systemClk = RCC_GetPLLOutputClock();
	}

	temp = (RCC->CFGR >> 4) & 0xF;

	if(temp < 8) {
		ahbp = 1;
	}

	else {
		ahbp = AHB_PreScaler[temp - 8];
	}

	temp = (RCC->CFGR >> 10) & 0x7;

	if(temp < 4) {
		apb1p = 1;
	}

	else {
		apb1p = APB_PreScaler[temp - 4];
	}

	pClk1 = (systemClk / ahbp) / apb1p;

	return pClk1;
}

uint32_t RCC_GetPCLK2Value(void) {
	uint32_t pClk2, systemClk;

	uint8_t clkSrc, temp, ahbp, apb2p;

	clkSrc = (RCC->CFGR >> 2) & 0x3;

	if(clkSrc == 0) {
		systemClk =  16000000;
	}

	else if(clkSrc == 1) {
		systemClk = 8000000;
	}

	else if(clkSrc == 2) {
		systemClk = RCC_GetPLLOutputClock();
	}

	temp = (RCC->CFGR >> 4) & 0xF;

	if(temp < 8) {
		ahbp = 1;
	}

	else {
		ahbp = AHB_PreScaler[temp - 8];
	}

	temp = (RCC->CFGR >> 13) & 0x7;

	if(temp < 4) {
		apb2p = 1;
	}

	else {
		apb2p = APB_PreScaler[temp - 4];
	}

	pClk2 = (systemClk / ahbp) / apb2p;

	return pClk2;
}
