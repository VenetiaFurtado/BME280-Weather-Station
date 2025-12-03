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
 *
 * @author  Venetia Furtado
 * @date    12/02/2025
 *
 */

#include <stdint.h>

void Init_SPI2(void);
uint8_t SPI_Read(const uint8_t register_addr);
uint8_t SPI_Write(const uint8_t register_addr, const uint8_t data);

#endif