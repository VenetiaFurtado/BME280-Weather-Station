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
 * @file    spi.c
 * @brief	SPI2 peripheral driver and utility functions.
 *         
 * @author  Venetia Furtado
 * @date    12/02/2025
 * Reference:
 * 1. https://github.com/alexander-g-dean/ESF/blob/master/ST/Code/ch8/SPI/main.c
 */

#include <stm32f091xc.h>
#include <stdio.h>
#include "utilities.h"

#define SCK          2
#define MISO         2
#define MOSI         2
#define MSTR_CLOCK   3
#define MSTR_MODE    1
#define CPHA         1
#define CPOL         1
#define MSB_FIRST    0
#define DATA         7
#define SPI2_ENABLE  1

/**
 * @brief Initializes SPI2 peripheral and associated GPIO pins.
 *
 * Configures GPIOB pins 12–15 for SPI2 alternate function (NSS, SCK, MISO, MOSI),
 * sets SPI2 as master with 12 MHz clock, CPOL=1, CPHA=0, MSB-first, 8-bit data,
 * and enables automatic NSS management.
 * Code referenced from:
 * https://github.com/alexander-g-dean/ESF/blob/master/ST/Code/ch8/SPI/main.c
 */
void Init_SPI2(void)
{
   // Clock gating for SPI2 and GPIO B
   RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
   RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

   // GPIO B pin 12 in alternate function 0 (SPI2) for NSS
   // Set mode field to 2 for alternate function
   MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER12, ESF_GPIO_MODER_ALT_FUNC);
   
   // Select SPI2 (AF = 0) for alternate function
   MODIFY_FIELD(GPIOB->AFR[1], GPIO_AFRH_AFSEL12, 0);

   // GPIO B pin 13, 14, 15 in alternate function 0 (SPI2) for SCK, MISO, MOSI
   // Set each mode field to 2 for alternate function
   MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER13, SCK); 
   MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER14, MISO); 
   MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER15, MOSI);

   // Select SPI2 (AF = 0) for alternate function
   MODIFY_FIELD(GPIOB->AFR[1], GPIO_AFRH_AFSEL13, 0);
   MODIFY_FIELD(GPIOB->AFR[1], GPIO_AFRH_AFSEL14, 0);
   MODIFY_FIELD(GPIOB->AFR[1], GPIO_AFRH_AFSEL15, 0);

   // Clock is divided by 4 (2^(BR+1)) 
   MODIFY_FIELD(SPI2->CR1, SPI_CR1_BR, MSTR_CLOCK);
   MODIFY_FIELD(SPI2->CR1, SPI_CR1_MSTR, MSTR_MODE); // Master mode

   MODIFY_FIELD(SPI2->CR1, SPI_CR1_SSM, 0); 
   MODIFY_FIELD(SPI2->CR1, SPI_CR1_SSI, 0); 
   MODIFY_FIELD(SPI2->CR2, SPI_CR2_SSOE, 1); 

   // Select first edge sample, active high clock
   MODIFY_FIELD(SPI2->CR1, SPI_CR1_CPHA, CPHA); 
   MODIFY_FIELD(SPI2->CR1, SPI_CR1_CPOL, CPOL);

   // Data is MSB first
   MODIFY_FIELD(SPI2->CR1, SPI_CR1_LSBFIRST, MSB_FIRST);
   // Data is 8 bits long
   MODIFY_FIELD(SPI2->CR2, SPI_CR2_DS, DATA);
   // RXNE when at least 1 byte in RX FIFO
   MODIFY_FIELD(SPI2->CR2, SPI_CR2_FRXTH, 1);
   // Have NSS pin asserted automatically
   //MODIFY_FIELD(SPI2->CR2, SPI_CR2_NSSP, 1);
   // Enable SPI
   //MODIFY_FIELD(SPI2->CR1, SPI_CR1_SPE, SPI2_ENABLE);
}

/**
 * @brief Sends a byte over SPI2 and receives a byte in return.
 *
 * @param d_out Byte to transmit.
 * @return Received byte from SPI2.
 * @note Code referenced from:
 * https://github.com/alexander-g-dean/ESF/blob/master/ST/Code/ch8/SPI/main.c
 */
uint8_t SPI_Send_Receive_Byte(uint8_t d_out)
{
   uint8_t d_in;
   // Wait until transmitter buffer is empty
   while ((SPI2->SR & SPI_SR_TXE) == 0)
      ;
   // Transmit d_out
   // Must tell compiler to use a byte write (not half-word)
   // by casting SPI1->DR into a pointer to a byte (uint8_t).
   // See STM32F0 Snippets (SPI_01_FullDuplexCommunications).
   *((uint8_t *)&(SPI2->DR)) = d_out;

   //need to write a dummy byte to generate clocks.
   //*((uint8_t *)&(SPI2->DR)) = 0xFF;

   // Wait until receiver is not empty
   while ((SPI2->SR & SPI_SR_RXNE) == 0)
      ;
   // Get d_in
   d_in = (uint8_t)SPI2->DR;
   return d_in;
}

/**
 * @brief Reads a single byte from a specified SPI register.
 *
 * This function enables the SPI2 peripheral, sends the register address,
 * receives the data byte from the SPI slave, and then disables the SPI2
 * peripheral.
 *
 * @param[in] register_addr  Address of the register to read from.
 * @return uint8_t           Value read from the specified register.
 */
uint8_t SPI_Read(const uint8_t register_addr)
{
   uint8_t val = 0;

   MODIFY_FIELD(SPI2->CR1, SPI_CR1_SPE, SPI2_ENABLE);

   SPI_Send_Receive_Byte(register_addr);
   val = SPI_Send_Receive_Byte(0x00);

   MODIFY_FIELD(SPI2->CR1, SPI_CR1_SPE, 0);

   return val;
}

/**
 * @brief Writes a single byte to a specified SPI register.
 *
 * This function enables the SPI2 peripheral, sends the register address,
 * writes the provided data byte to the SPI slave, and then disables the SPI2
 * peripheral. The function returns the value received during the last SPI transfer.
 *
 * @param[in] register_addr  Address of the register to write to.
 * @param[in] data           Data byte to write to the register.
 * @return uint8_t           Value received from the SPI slave during the transfer.
 */
uint8_t SPI_Write(const uint8_t register_addr, const uint8_t data)
{
   uint8_t val = 0;

   MODIFY_FIELD(SPI2->CR1, SPI_CR1_SPE, SPI2_ENABLE);

   SPI_Send_Receive_Byte(register_addr);
   val = SPI_Send_Receive_Byte(data);

   MODIFY_FIELD(SPI2->CR1, SPI_CR1_SPE, 0);

   return val;
}
