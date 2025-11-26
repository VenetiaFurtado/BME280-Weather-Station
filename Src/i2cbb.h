#ifndef I2C_I2C_H_
#define I2C_I2C_H_

/*******************************************************************************
 * Author - Venetia Furtado
 * Final Project: Health Monitoring System
 * ECEN 5613 - Spring 2024 - Prof. McClure
 * University of Colorado Boulder
 * Revised 04/26/2024
*
********************************************************************************
  * This file contains all forward definitions of the functions that implement 
  * the I2C protocol.
*******************************************************************************/
/******************************************************************************/
//                            DEFINES
/******************************************************************************/
#include <stdint.h>

#define SCL         8
#define SDA         9
#define MFIO        8
#define RSTN        9

#define CMD_DELAY   10  // Milliseconds

#define FAIL        -1
#define SUCCESS      0

typedef enum
{
    READ = 1,
    WRITE = 0
} Operation;

/******************************************************************************/
//                            FORWARD DECLARATIONS
/******************************************************************************/
/**
 * @brief Logic to initialize the GPIO pins to be used as  SCL, SDA, MFIO, RSTN
 * 
 */
void i2cInitGpio();

/**
 * @brief Logic to provide delay
 * 
 * @param val 
 */
//void delay(int val);

/**
 * @brief Toggles the clock
 * 
 */
void clock();

/**
 * @brief Checks if an acknowledgement has been given by the peripheral
 * 
 * @return int SUCCESS or FAIL
 */
int checkAcknowledgement();

/**
 * @brief Logic for starting the I2C communication
 * 
 */
void start();

/**
 * @brief Logic to stop the I2C communication
 * 
 */
void stop();

/**
 * @brief Logic to reverse a byte; LSB of input become MSB of output.
 * @param input
 * @return uint8_t 
 */
uint8_t reverse(uint8_t input);

/**
 * @brief Logic for sending the Control Byte
 * 
 * @param op enum READ/WRITE
 * @param addr address to be READ/WRITE from 
 */
void sendControlByte(const Operation op, uint8_t deviceId);

/**
 * @brief Logic to set/reset the bit (pin) of the BSRR
 * 
 * @param pin 
 * @param val 1: set; 0:reset
 */
void i2cSetPin(int pin, uint8_t val);

/**
 * @brief Write a Byte over I2C
 * 
 * @param byte 
 */
void i2cByteWrite(uint8_t byte);

/**
 * @brief Reads a byte from the I2C transaction
 * 
 * @return uint8_t 
 */
uint8_t i2cByteRead();

/**
 * @brief Logic to send Acknowledge(from Master)
 * 
 */
void sendAcknowledge();

/**
 * @brief Logic to send Nack
 * 
 */
void sendNAcknowledge();

#endif
