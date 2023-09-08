/*
 * 010i2c_master_tx.c
 *
 *  Created on: Jul 26, 2023
 *      Author: barissezer
 */
#include<stdio.h>
#include<string.h>
#include "stm32f407xx.h"

extern void initialise_monitor_handles();


#define MY_ADDR		0x61
#define SLAVE_ADDR 	0x68

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}

//rcv buffer
uint8_t rcv_buf[32];


/*
 * PB6 -> SCL
 * PB9 -> SDA
 */

I2C_Handle_t I2C1Handle;

void I2C1_GPIOInits(void)
{
	GPIO_Handle_t I2CPins;

	I2CPins.pGPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//scl
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&I2CPins);

	//sda
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&I2CPins);
}

void I2C1_Inits(void)
{
	I2C1Handle.pI2Cx = I2C1;

	I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress= MY_ADDR;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;

	I2C_Init(&I2C1Handle);

}


void GPIO_ButtonInit()
{
	GPIO_Handle_t GPIOButtonHandle;
	//button config
	GPIOButtonHandle.pGPIOx = GPIOA;
	GPIOButtonHandle.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOButtonHandle.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOButtonHandle.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIOButtonHandle.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	GPIO_Init(&GPIOButtonHandle);
}

int main(void)
{

	uint8_t commandCode;

	uint8_t len;

	initialise_monitor_handles();

	printf("Application is running\n");

	GPIO_ButtonInit();

	//I2C pin inits
	I2C1_GPIOInits();

	//I2C peripheral configuration
	I2C1_Inits();

	//enable i2c peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	//ack bit can be configured by the software only after PE becomes 1
	I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);


	//wait for button press
	while(1)
	{
		//wait until the button is pressed
		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));
		//delay to solve debouncing problem
		delay();

		commandCode = 0x51;

		I2C_MasterSendData(&I2C1Handle, &commandCode, 1, SLAVE_ADDR, I2C_SR_ENABLE);

		I2C_MasterReceiveData(&I2C1Handle, &len, 1, SLAVE_ADDR, I2C_SR_ENABLE);

		commandCode = 0x52;

		I2C_MasterSendData(&I2C1Handle, &commandCode, 1, SLAVE_ADDR, I2C_SR_ENABLE);

		I2C_MasterReceiveData(&I2C1Handle, rcv_buf, len, SLAVE_ADDR, I2C_SR_DISABLE);

		rcv_buf[len+1] = '\0';

		printf("Data : %s\n", rcv_buf);

	}



}
