/*
 * 001led_toggle.c
 *
 *  Created on: Jun 25, 2023
 *      Author: barissezer
 */


#include "stm32f407xx.h"

void delay()
{
	for(uint32_t i = 0; i < 500000; i++);
}

int main(void){
	GPIO_Handle_t GPIOLedHandle;
	GPIOLedHandle.pGPIOx = GPIOD;


	GPIO_PinCfg_t GPIOD_Config;

	GPIOD_Config.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIOD_Config.GPIO_PinMode = GPIO_MODE_OUT;
	GPIOD_Config.GPIO_PinOPType = GPIO_OP_TYPE_PP; //Push-pull config
	GPIOD_Config.GPIO_PinPuPdControl = GPIO_NO_PUPD; //push-pull config
	//GPIOD_Config.GPIO_PinOPType = GPIO_OP_TYPE_OD;  //Open drain config
	//GPIOD_Config.GPIO_PinPuPdControl = GPIO_NO_PUPD; //open drain config
	GPIOD_Config.GPIO_PinSpeed = GPIO_SPEED_FAST;

	GPIOLedHandle.GPIO_PinCfg = GPIOD_Config;

	GPIO_PClkControl(GPIOD, ENABLE);

	GPIO_Init(&GPIOLedHandle);

	while(1)
	{
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
		delay();
	}

	return 0;
}
