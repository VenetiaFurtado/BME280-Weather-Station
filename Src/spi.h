#ifndef __SPI_H__
#define __SPI_H__

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
 * @file    spi.h
 * @brief	Header file for SPI2 peripheral driver.
 *          Declares initialization, full-duplex byte transfer,
 *          and loopback test functions.
 *
 * @author  Venetia Furtado
 * @date    11/03/2025
 *
 */

#include <stdint.h>

void Init_SPI2(void);
uint8_t SPI_Send_Receive_Byte(uint8_t d_out);
uint8_t spi_read();
void Test_SPI_Loopback(void);

#endif