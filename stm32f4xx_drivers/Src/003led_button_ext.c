/*
 * 002led_button.c
 *
 *  Created on: Jun 25, 2023
 *      Author: barissezer
 */

#include "stm32f407xx.h"

#define LOW 0
#define BTN_PRESSED LOW

void delay()
{
	for(uint32_t i = 0; i < 500000/2; i++);
}

int main(void){
	GPIO_Handle_t GPIOLedHandle;
	GPIOLedHandle.pGPIOx = GPIOD;

	GPIO_Handle_t GPIOButtonHandle;
	GPIOButtonHandle.pGPIOx = GPIOB;

	GPIO_PinCfg_t GPIOB_Config;

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
	GPIOB_Config.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOB_Config.GPIO_PinMode = GPIO_MODE_IN;
	GPIOB_Config.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIOB_Config.GPIO_PinSpeed = GPIO_SPEED_FAST;


	GPIOButtonHandle.GPIO_PinCfg = GPIOB_Config;

	GPIO_PClkControl(GPIOB, ENABLE);

	GPIO_Init(&GPIOButtonHandle);

	while(1)
	{
		uint8_t status = GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_12);
		if(status == BTN_PRESSED){
			delay();
			GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
		}

	}

	return 0;
}
