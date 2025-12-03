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
 * @brief	Forward declaration of the functions in bme280.c
 * Source:
 * 1. https://github.com/sparkfun/SparkFun_BME280_Arduino_Library/tree/870c17da1f4c76561e14b8ffcc7cdffd63136e10/src
 * 2. https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf
 * @date    12/02/2025
 *
 */

#include <stdint.h>

//#define RUN_WITH_SPI 0


// BME280 measurement data
typedef struct {
    float temperature;  // °C
    float pressure;     // hPa
    float humidity;     // %
} BME280_Data;

uint8_t BME280_Init(void);
void BME280_ReadAll(BME280_Data *data);


#endif