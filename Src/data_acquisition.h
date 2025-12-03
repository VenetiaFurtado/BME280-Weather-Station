#ifndef __DATA_ACQUISITION_H
#define __DATA_ACQUISITION_H
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
 * @file    data_acquisition.h
 * @brief	Forward declarations for functions in the data_acquisition.c file
 *
 * @author  Venetia Furtado
 * @date    12/02/2025
 *
 */
#include "bme280.h"

void Init_DataAcquisition();
void acquire_data(BME280_Data* data);
float get_avg_temp();

 #endif