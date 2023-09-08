/*
 * 006spi_tx_testing.c
 *
 *  Created on: Feb 10, 2019
 *      Author: admin
 */

#include <stdio.h>
#include <string.h>
#include "stm32f407xx.h"

extern void initialise_monitor_handles();
/*
 * PB14 --> SPI2_MISO
 * PB15 --> SPI2_MOSI
 * PB13 -> SPI2_SCLK
 * PB12 --> SPI2_NSS
 * ALT function mode : 5
 */

//command codes
#define COMMAND_LED_CTRL		0x50
#define COMMAND_SENSOR_READ		0x51
#define COMMAND_LED_READ		0x52
#define COMMAND_PRINT			0x53
#define COMMAND_ID_READ			0x54

#define LED_ON					1
#define LED_OFF					0

//ardunio analog pins
#define ANALOG_PIN0				0
#define ANALOG_PIN1				1
#define ANALOG_PIN2				2
#define ANALOG_PIN3				3
#define ANALOG_PIN4				4

//ardunio led
#define LED_PIN		9

void SPI2_GPIOInits(void)
{
	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	//MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);

	//MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPIPins);


	//NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPIPins);


}

void SPI2_Inits(void)
{

	SPI_Handle_t SPI2handle;

	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV8;//generates sclk of 8MHz
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_DI; //hardware slave management enabled for NSS pin

	SPI_Init(&SPI2handle);
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

void delay()
{
	for(uint32_t i = 0; i < 500000/2; i++);
}


uint8_t SPI_VerifyResponse(uint8_t ack_byte)
{
	if(ack_byte == 0xF5)
	{
		//ack
		return 1;
	}
	//nack
	return 0;
}
int main(void)
{

	uint8_t dummy_write = 0xff;
	uint8_t dummy_read;

	initialise_monitor_handles();

	printf("Application is running\n");

	GPIO_ButtonInit();

	//this function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI2_GPIOInits();

	//This function is used to initialize the SPI2 peripheral parameters
	SPI2_Inits();

	printf("SPI init\n");

	SPI_SSOEConfig(SPI2, ENABLE);

	while(1)
	{
		//wait until the button is pressed
		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));
		//delay to solve debouncing problem
		delay();

		//enable the SPI2 peripheral
		SPI_PeripheralControl(SPI2,ENABLE);

		//1. CMD_LED_CTRL  <pin no(1)>  <value(1)>
		uint8_t command_code = COMMAND_LED_CTRL;
		uint8_t ack_byte;
		uint8_t args[2];
		SPI_SendData(SPI2, &command_code, 1);

		//do dummy read to clear RXNE bit
		SPI_ReceiveData(SPI2, &dummy_read, 1, 0);

		//send one dummy byte the get the response ffrom the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//Read the data from DR
		SPI_ReceiveData(SPI2, &ack_byte, 1, 0);

		//verify if you recieved ack or not
		if(SPI_VerifyResponse(ack_byte))
		{
			//send arguments
			args[0] = LED_PIN;
			args[1] = LED_ON;
			SPI_SendData(SPI2, args, 2);

			printf("COMMAND_LED_CTRL executed\n");

		}
		//End of COMMAND_LED_CTRL

		//2. CMD_SENSOR_READ   <analog pin number(1) >

		//wait until the button is pressed
		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		//delay to solve debouncing problem
		delay();

		command_code = COMMAND_SENSOR_READ;

		SPI_SendData(SPI2, &command_code, 1);

		//do dummy read to clear RXNE bit
		SPI_ReceiveData(SPI2, &dummy_read, 1, 0);

		//send one dummy byte the get the response ffrom the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//Read the data from DR
		SPI_ReceiveData(SPI2, &ack_byte, 1, 0);

		//verify if you recieved ack or not
		if(SPI_VerifyResponse(ack_byte))
		{
			//send arguments
			args[0] = ANALOG_PIN0;

			SPI_SendData(SPI2, args, 1);
			//do dummy read to clear RXNE bit
			SPI_ReceiveData(SPI2, &dummy_read, 1, COMMAND_SENSOR_READ);

			//send one dummy byte the get the response ffrom the slave
			SPI_SendData(SPI2, &dummy_write, 1);

			uint8_t analog_read;

			//Read the data from DR
			SPI_ReceiveData(SPI2, &analog_read, 1, COMMAND_SENSOR_READ);

			printf("COMMAND_SENSOR_READ executed, %d\n", analog_read);

		}


		//end of COMMAND_SENSOR_READ

		//3.  CMD_LED_READ 	 <pin no(1) >
		//wait until the button is pressed
		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		//delay to solve debouncing problem
		delay();

		command_code = COMMAND_LED_READ;

		SPI_SendData(SPI2, &command_code, 1);

		//do dummy read to clear RXNE bit
		SPI_ReceiveData(SPI2, &dummy_read, 1, 0);

		//send one dummy byte the get the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//Read the data from DR
		SPI_ReceiveData(SPI2, &ack_byte, 1, 0);

		//verify if you recieved ack or not
		if(SPI_VerifyResponse(ack_byte))
		{
			args[0] = LED_PIN;

			SPI_SendData(SPI2, args, 1);

			SPI_ReceiveData(SPI2, &dummy_read, 1, 0);

			SPI_SendData(SPI2, &dummy_write, 1);

			uint8_t led_status;

			SPI_ReceiveData(SPI2, &led_status, 1, 0);

			printf("COMMAND_LED_READ executed, %d\n", led_status);


		}

		//4. CMD_PRINT 		<len(2)>  <message(len) >
		//wait until the button is pressed
		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		//delay to solve debouncing problem
		delay();

		command_code = COMMAND_PRINT;

		SPI_SendData(SPI2, &command_code, 1);

		//do dummy read to clear RXNE bit
		SPI_ReceiveData(SPI2, &dummy_read, 1, 0);

		//send one dummy byte the get the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//Read the data from DR
		SPI_ReceiveData(SPI2, &ack_byte, 1, 0);

		uint8_t message[] = "Hello ! How are you ??";
		//verify if you recieved ack or not
		if(SPI_VerifyResponse(ack_byte))
		{
			args[0] = strlen((char*)message);

			SPI_SendData(SPI2, args, 1);

			SPI_ReceiveData(SPI2, &dummy_read, 1, 0);

			for(int i = 0; i < args[0]; i++)
			{
				SPI_SendData(SPI2, &message[i], 1);

				SPI_ReceiveData(SPI2, &dummy_read, 1, 0);
			}

			printf("COMMAND_PRINT executed\n");
		}

		//5. CMD_ID_READ
		//wait until the button is pressed
		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		//delay to solve debouncing problem
		delay();

		command_code = COMMAND_ID_READ;

		SPI_SendData(SPI2, &command_code, 1);

		//do dummy read to clear RXNE bit
		SPI_ReceiveData(SPI2, &dummy_read, 1, 0);

		//send one dummy byte the get the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//Read the data from DR
		SPI_ReceiveData(SPI2, &ack_byte, 1, 0);

		uint8_t id[11];
		uint32_t i = 0;
		if(SPI_VerifyResponse(ack_byte))
		{
			for(i = 0; i < 10; i++)
			{
				SPI_SendData(SPI2, &dummy_write, 1);

				SPI_ReceiveData(SPI2, &id[i], 1, 0);
			}
			id[10] = '\0';

			printf("COMMAND_ID_READ executed, %s\n", id);

		}

		//lets confirm SPI is not busy
		while( SPI_GetFlagStatus(SPI2,SPI_BUSY_FLAG) );

		//Disable the SPI2 peripheral
		SPI_PeripheralControl(SPI2,DISABLE);

		printf("SPI communication is closed\n");


	}

	return 0;

}
