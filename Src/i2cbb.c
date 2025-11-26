/*******************************************************************************
 * Author - Venetia Furtado
 * Final Project: Health Monitoring System
 * ECEN 5613 - Spring 2024 - Prof. McClure
 * University of Colorado Boulder
 * Revised 04/26/2024
 *
 ********************************************************************************
 * This file contains the function to implement I2C communication for the
 * MAXHUB module
 ******************************************************************************/

/******************************************************************************/
//                            INCLUDES
/******************************************************************************/
#include "i2cbb.h"
#include "utilities.h"
#include <stm32f091xc.h>


#define NOP for(int k = 0; k < 5000; k++)

/*MODER[7:6]   = 01
OTYPER[5:4]  = 01
OSPEEDR[3:2] = 01
PUPDR[1:0]   = 01*/
#define GPIO_OUTPUT_OPENDRAIN 0x51
/******************************************************************************/
//                            FUNCTION DEFINITIONS
/******************************************************************************/


/**
 * @brief Logic to initialize the GPIO pins to be used as  SCL, SDA, MFIO, RSTN
 * 
 */
void i2cInitGpio()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

   MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER8, ESF_GPIO_MODER_OUTPUT);
   MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER9, ESF_GPIO_MODER_OUTPUT);

   GPIOB->OTYPER |= GPIO_OTYPER_OT_8;
   GPIOB->OTYPER |= GPIO_OTYPER_OT_9;

   MODIFY_FIELD(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEEDR8, 0);
   MODIFY_FIELD(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEEDR9, 0);

   MODIFY_FIELD(GPIOB->PUPDR, GPIO_PUPDR_PUPDR8, 1);
   MODIFY_FIELD(GPIOB->PUPDR, GPIO_PUPDR_PUPDR9, 1);
}

/**
 * @brief Reads the status of a input GPIO pin
 * 
 * @param pin 
 * @return uint8_t 
 */
uint8_t i2cReadPin(int pin)
{
	uint8_t val = (GPIOB->IDR >> pin) & 0x01;
	//INFO_LOG("GPIOB->IDR = %lX", GPIOB->IDR);
	return val;
}

/**
 * @brief Logic to set/reset the bit (pin) of the BSRR
 * 
 * @param pin 
 * @param val 1: set; 0:reset
 */
void i2cSetPin(int pin, uint8_t val)
{
	uint32_t pinMask = (0b01 << pin);
	if (val == 1)
	{
		GPIOB->BSRR |= pinMask;
	}
	else
	{
		GPIOB->BSRR |= pinMask << 16;
	}
}

/**
 * @brief Toggles the clock
 *
 */
void clock()
{
	NOP;
	// SCL = 1;
	i2cSetPin(SCL, 1);
	NOP;
	NOP;
	// SCL = 0;
	i2cSetPin(SCL, 0);
	NOP;
}

/**
 * @brief Logic for starting the I2C communication
 *
 */
void start()
{
	// SDA = 1;
	i2cSetPin(SDA, 1);
	NOP;
	// SCL = 1;
	i2cSetPin(SCL, 1);
	NOP;
	// SDA = 0;
	i2cSetPin(SDA, 0);
	NOP;
	NOP;
	// SCL = 0;
	i2cSetPin(SCL, 0);
}

/**
 * @brief Logic to stop the I2C communication
 *
 */
void stop()
{
	// SDA = 0;
	i2cSetPin(SDA, 0);
	NOP;
	NOP;
	// SCL = 1;
	i2cSetPin(SCL, 1);
	NOP;
	NOP;
	// SDA = 1;
	i2cSetPin(SDA, 1);
	NOP;
	NOP;
}

/**
 * @brief Logic to send Acknowledge(from Master)
 * 
 */
void sendAcknowledge()
{
	// SDA = 0;
	i2cSetPin(SDA, 0);
	NOP;
	NOP;
	clock();
	// SDA = 1;
	i2cSetPin(SDA, 1);
	NOP;
	NOP;
}

/**
 * @brief Logic to send Nack
 * 
 */
void sendNAcknowledge()
{
	// SDA = 0;
	i2cSetPin(SDA, 1);
	NOP;
	NOP;
	clock();
	NOP;
	NOP;
}

/**
 * @brief Logic to reverse a byte; LSB of input become MSB of output.
 * @param input
 * @return uint8_t
 */
uint8_t reverse(uint8_t input)
{
	uint8_t output = 0;

	for (int i = 0; i < 8; i++)
	{
		output = output << 1;
		// Bitwise AND-ing input with 0x01 to obtain its LSB and OR with output
		output = output | (input & 0x01);
		input = input >> 1;
	}
	return output;
}

/**
 * @brief Logic for sending the Control Byte
 *
 * @param op enum READ/WRITE
 * @param deviceId device ID to be read/write from
 */
void sendControlByte(const Operation op, uint8_t deviceId)
{
	// Sending the device address
	for (int i = 0; i < 7; i++)
	{
		// sets SDA pin high
		i2cSetPin(SDA, deviceId & 0x01);
		clock();
		deviceId = deviceId >> 1;
	}
   
	//Selects Read or Write operation
	i2cSetPin(SDA, op);
	clock();
}

/**
 * @brief Checks if an acknowledgement has been given by the peripheral
 *
 * @return int SUCCESS or FAIL
 */
int checkAcknowledgement()
{
	// check to see if the device is acking by pulling SDA low
	i2cSetPin(SDA, 1);
	NOP;
	NOP;
	i2cSetPin(SCL, 1);
	NOP;
	// Capturing value at SDA
	uint8_t value = i2cReadPin(SDA);
	NOP;
	// SCL = 0;
	i2cSetPin(SCL, 0);
	NOP;
	if (value != 0)
	{
		return FAIL;
	}
	return SUCCESS;
}

/**
 * @brief Write a Byte over I2C
 * 
 * @param byte 
 */
void i2cByteWrite(uint8_t byte)
{
	uint8_t byteMsbFirst = reverse(byte);

	//Sending byteMsbFirst
  for (int i = 0; i < 8; i++)
  {
    //SDA = byteMsbFirst & 0x01;
	 i2cSetPin(SDA, byteMsbFirst & 0x01);
    byteMsbFirst >>= 1;
    clock();
  }
}

/**
 * @brief Reads a byte from the I2C transaction
 * 
 * @return uint8_t 
 */
uint8_t i2cByteRead()
{
	uint8_t dataByte = 0;
	for (int i = 0; i < 8; i++)
	{
		// SCL = 1;
		i2cSetPin(SCL, 1);
		// Bit packing each bit into dataByte to give MCU the data from requested address from MSB to LSB(little endian)
		uint8_t sdaReadData = i2cReadPin(SDA);
		dataByte = (dataByte << 1) | sdaReadData;
		NOP;
		//SCL = 0;
		i2cSetPin(SCL, 0);
		NOP;
	}
	return dataByte;
}
