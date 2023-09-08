/*
 * 002led_button.c
 *
 *  Created on: Jun 25, 2023
 *      Author: barissezer
 */

#include "stm32f407xx.h"

#define HIGH 1
#define BTN_PRESSED HIGH

void delay()
{
	for(uint32_t i = 0; i < 500000/2; i++);
}

int main(void){
	GPIO_Handle_t GPIOLedHandle;
	GPIOLedHandle.pGPIOx = GPIOD;

	GPIO_Handle_t GPIOButtonHandle;
	GPIOButtonHandle.pGPIOx = GPIOA;

	GPIO_PinCfg_t GPIOA_Config;

	GPIO_PinCfg_t GPIOD_Config;


	//led config
	GPIOD_Config.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIOD_Config.GPIO_PinMode = GPIO_MODE_OUT;
	GPIOD_Config.GPIO_PinOPType = GPIO_OP_TYPE_PP; //Push-pull config
	GPIOD_Config.GPIO_PinPuPdControl = GPIO_NO_PUPD; //push-pull config
	//GPIOD_Config.GPIO_PinOPType = GPIO_OP_TYPE_OD;  //Open drain config
	//GPIOD_Config.GPIO_PinPuPdControl = GPIO_NO_PUPD; //open drain config

	GPIOLedHandle.GPIO_PinCfg = GPIOD_Config;

	GPIO_PClkControl(GPIOD, ENABLE);

	GPIO_Init(&GPIOLedHandle);


	//button config
	GPIOA_Config.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOA_Config.GPIO_PinMode = GPIO_MODE_IN;
	GPIOA_Config.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIOA_Config.GPIO_PinSpeed = GPIO_SPEED_FAST;


	GPIOButtonHandle.GPIO_PinCfg = GPIOA_Config;

	GPIO_PClkControl(GPIOA, ENABLE);

	GPIO_Init(&GPIOButtonHandle);

	while(1)
	{
		uint8_t status = GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0);
		if(status == BTN_PRESSED){
			delay();
			GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
		}

	}

	return 0;
}
