/**
 * @file    data_acquisition.c
 * @brief	Data acquisition and temperature averaging module.
 *
 * @author  Venetia Furtado
 * @date    12/02/2025
 *
 */
#include <stm32f091xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "data_acquisition.h"
#include "buffer.h"
#include "utilities.h"
#include "log.h"

BufferType data_buffer;
double running_sum_temp;
float avg_temp;

/**
 * @brief Initializes the data acquisition subsystem.
 *
 * This function prepares the global data buffer used for storing sensor
 * readings by populating it with predefined default values. It is
 * called before any data collection or processing occurs.
 */
void Init_DataAcquisition()
{
   init_buffer_with_default_val(&data_buffer);
}

/**
 * @brief Acquires a new BME280 sensor sample and updates buffered data.
 *
 * This function reads the latest temperature, pressure, and humidity values
 * from the BME280 sensor and stores the measurement in a circular buffer.
 * If the buffer is full, the oldest sample is removed to maintain the fixed
 * size window. The function also maintains a running sum of temperatures,
 * allowing efficient computation of a moving average without recalculating
 * over the entire buffer.
 *
 * @param[out] data Pointer to a BME280_Data structure that will be filled
 *                  with the latest sensor measurement.
 */
void acquire_data(BME280_Data* data)
{
   BME280_ReadAll(data);

   if (write_to_buffer(&data_buffer, data) == -1)
   {
      INFO_LOG("Write to buffer failed!!");
   }

   running_sum_temp += data->temperature;

   if(cbfifo_length(&data_buffer) == 60)
   {
      BME280_Data old_sample;
      read_from_buffer(&data_buffer, &old_sample);
      running_sum_temp -= old_sample.temperature;
   }
   avg_temp = running_sum_temp/cbfifo_length(&data_buffer); 
}

/**
 * @brief Acquires a new BME280 sensor sample and updates the running average.
 *
 * This function reads a temperature/humidity/pressure sample from the BME280
 * sensor, stores it into the circular buffer, and maintains a 60-sample moving
 * average of the temperature. When the buffer reaches its maximum capacity
 * (60 samples), the oldest sample is removed and its contribution is subtracted
 * from the running sum.
 *
 * @param[in] data Pointer to a BME280_Data structure where the newly acquired
 *                 sensor values will be stored.
 */
float get_avg_temp()
{
   return avg_temp;
}
