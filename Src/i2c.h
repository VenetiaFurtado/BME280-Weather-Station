#ifndef __I2C_H__
#define __I2C_H__

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
 * @file    i2c.h
 * @brief	This header declares functions for initializing the I2C1 peripheral
 * and performing blocking read and write operations to I2C slave devices.
 * It is used for communication with the BME280.
 *
 * @author  Venetia Furtado
 * @date    12/02/2025
 *
 */
#include <stdint.h>

void I2C_Init(void);
void I2C_ReadReg(uint8_t dev_adx, uint8_t reg_adx, uint8_t *bufp, uint16_t data_len);
void I2C_WriteReg(uint8_t dev_adx, uint8_t reg_adx, const uint8_t *bufp, uint16_t data_len);

#endif