/*******************************************************************************
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * Venetia Furtado and the University of Colorado are not liable for
 * any misuse of this material.
 * ****************************************************************************/
/**
 * @file    i2c.c
 * @brief   I2C1 peripheral initialization and read/write functions for STM32F0.
 *
 * This file provides functions to initialize the I2C1 peripheral and perform
 * blocking read and write operations to I2C slave devices. It is used for
 * communication with sensors such as the BME280.
 *
 * @author  Venetia Furtado
 * @date    11/25/2025
 * Reference:
 * 1. https://github.com/alexander-g-dean/ESF/blob/master/ST/Code/ch8/I2C/main.c
 * 2. BME280 Datasheet
 */

#include <stm32f091xc.h>
#include <stdio.h>
#include "utilities.h"

/**
 * @brief Initializes the I2C1 peripheral for communication with the BME280 sensor.
 *
 * This function configures the I2C1 peripheral and associated GPIO pins
 * on STM32F0 for standard mode (100 kHz) operation. It performs the following steps:
 *  - Enables clock gating for I2C1 and GPIO port B.
 *  - Configures PB8 (SCL) and PB9 (SDA) as alternate function open-drain pins with pull-ups.
 *  - Resets I2C1 peripheral.
 *  - Sets timing register for 100 kHz standard mode with 48 MHz system clock.
 *  - Configures CSB (PB12) and SDO (PB14) pins to select I2C interface and address 0x76.
 *  - Enables I2C1 peripheral.
 */
void I2C_Init(void)
{
   // Clock gating for I2C1 and GPIO B
   RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
   RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

   // GPIO B pin 8 and 9 in alternate function
   MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER8, ESF_GPIO_MODER_ALT_FUNC);
   MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER9, ESF_GPIO_MODER_ALT_FUNC);
   // Select I2C1 (AF = 1) as alternate function
   MODIFY_FIELD(GPIOB->AFR[1], GPIO_AFRH_AFSEL8, 1);
   MODIFY_FIELD(GPIOB->AFR[1], GPIO_AFRH_AFSEL9, 1);

   // Configure as open-drain
   GPIOB->OTYPER |= (GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9);

   // Enable pull-up resistors
   GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9);
   GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR8_0 | GPIO_PUPDR_PUPDR9_0);

   // Reset I2C1
   RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
   RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

   // Configure I2C1 timing for 100kHz with 48MHz clock
   // These values are for standard mode (100kHz)
   I2C1->TIMINGR = 0x10805E89;

   // CSB must be connected to VDDIO to select I²C interface.
   //Set PB12 high for CSB
   MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER12, ESF_GPIO_MODER_OUTPUT);
   GPIOB->BSRR |= GPIO_BSRR_BS_12;

   //Connecting SDO to GND results in slave address 1110110 (0x76)
   //Set PB14 low for SDO
   MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER14, ESF_GPIO_MODER_OUTPUT);
   GPIOB->BSRR |= GPIO_BSRR_BR_14;

   // I2C1 Configuration
   I2C1->CR1 = 0;              // Default configuration, peripheral disabled
   I2C1->CR2 = 0;              // Default configuration, 7 bit addressing
   
   MODIFY_FIELD(I2C1->CR1, I2C_CR1_PE, 1); // Enable peripheral
}

/**
 * @brief Writes data to a register of an I2C slave device.
 *
 * This function performs a blocking write operation over I2C1 to a
 * specified device and register. It sends the START condition, device
 * address, register address, data bytes, and finally issues a STOP condition.
 *
 * @param[in] dev_adx   7-bit I2C slave device address.
 * @param[in] reg_adx   Register address in the slave device to write to.
 * @param[in] bufp      Pointer to the buffer containing data to be sent.
 * @param[in] data_len  Number of bytes to write from the buffer.
 */
void I2C_WriteReg(uint8_t dev_adx, uint8_t reg_adx, const uint8_t* bufp, uint16_t data_len)
{
   uint32_t tmp;
   // -- Send START, Device Address, Write Command --
   tmp = 0;
   MODIFY_FIELD(tmp, I2C_CR2_SADD, dev_adx << 1);
   MODIFY_FIELD(tmp, I2C_CR2_RD_WRN, 0);            // Write
   MODIFY_FIELD(tmp, I2C_CR2_NBYTES, data_len + 1); // data bytes + reg. adx.
   // Set START to start transfer
   MODIFY_FIELD(tmp, I2C_CR2_START, 1);
   I2C1->CR2 = tmp;
   // Wait until START is cleared by hardware
   while (I2C1->CR2 & I2C_CR2_START)
      ;
   // -- Send Register Address --
   I2C1->TXDR = reg_adx;
   // Wait until transmitter empty
   while (!(I2C1->ISR & I2C_ISR_TXE))
      ;
   // -- Send Data --
   while (data_len--)
   {
      I2C1->TXDR = *bufp;
      bufp++;
      while (!(I2C1->ISR & I2C_ISR_TXE))
         ;
   }
   // -- Send Stop --
   MODIFY_FIELD(I2C1->CR2, I2C_CR2_STOP, 1);
}

/**
 * @brief Intended for testing and verification of the I2C_WriteReg() function.
 *
 * This function demonstrates writing a sequence of bytes to a specific
 * register of an I2C slave device. It writes 5 zero bytes to register
 * 0x06 of the device with address 0x6B.
 */
void test_I2CWrite(void)
{
   uint8_t data[] = {0, 0, 0, 0, 0};

   I2C_WriteReg(0x6B, 0x06, data, 5);
}

/**
 * @brief Reads data from a register of an I2C slave device.
 *
 * This function performs a blocking read operation over I2C1 from a
 * specified device and register. It performs the following steps:
 *  1. Sends a START condition and writes the device address with write mode.
 *  2. Sends the target register address.
 *  3. Sends a repeated START condition with the device address in read mode.
 *  4. Reads the specified number of bytes into the provided buffer.
 *  5. Issues a STOP condition to end the transaction.
 *
 * @param[in]  dev_adx   7-bit I2C slave device address.
 * @param[in]  reg_adx   Register address in the slave device to read from.
 * @param[out] bufp      Pointer to the buffer to store the read data.
 * @param[in]  data_len  Number of bytes to read.
 */
void I2C_ReadReg(uint8_t dev_adx, uint8_t reg_adx, uint8_t *bufp, uint16_t data_len)
{
   uint32_t tmp;
   // -- Send START, Device Address, Write Command --
   tmp = 0;
   MODIFY_FIELD(tmp, I2C_CR2_SADD, dev_adx<<1);
   MODIFY_FIELD(tmp, I2C_CR2_RD_WRN, 0); // First write addresses
   MODIFY_FIELD(tmp, I2C_CR2_NBYTES, 1); // 1 byte: register address
   MODIFY_FIELD(tmp, I2C_CR2_START, 1);  // Start transfer
   I2C1->CR2 = tmp;
   while (I2C1->CR2 & I2C_CR2_START) // Wait until START ends
      ;
   // -- Write Register Address --
   I2C1->TXDR = reg_adx;
   while (!(I2C1->ISR & I2C_ISR_TXE)) // Wait until sent
      ;
   // -- Send Repeated START, Device Address, Read Command--
   tmp = I2C1->CR2;
   MODIFY_FIELD(tmp, I2C_CR2_SADD, dev_adx << 1);
   MODIFY_FIELD(tmp, I2C_CR2_RD_WRN, 1);        // Then read data
   MODIFY_FIELD(tmp, I2C_CR2_NBYTES, data_len); // Data byte count
   MODIFY_FIELD(tmp, I2C_CR2_START, 1);         // Start transfer
   I2C1->CR2 = tmp;
   // -- Read Data --
   while (data_len--)
   {
      while (!(I2C1->ISR & I2C_ISR_RXNE)) // Await data rx
         ;
      *bufp++ = I2C1->RXDR; // Read RXDR, advance pointer
   }
   // -- Write Stop --
   MODIFY_FIELD(I2C1->CR2, I2C_CR2_STOP, 1);
}
