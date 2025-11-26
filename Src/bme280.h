#ifndef __BME280_H__
#define __BME280_H__

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
 * @file    bme280.h
 * @brief	Header file for SPI2 peripheral driver.
 *          Declares initialization, full-duplex byte transfer,
 *          and loopback test functions.
 *
 * @author  Venetia Furtado
 * @date    11/03/2025
 *
 */

#include <stdint.h>

// BME280 measurement data
typedef struct {
    float temperature;  // °C
    float pressure;     // hPa
    float humidity;     // %
} BME280_Data;

uint8_t BME280_Init(void);
void BME280_ReadAll(BME280_Data *data);


#endif