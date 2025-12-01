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

void Init_DataAcquisition()
{
   init_buffer_with_default_val(&data_buffer);
}


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

   //INFO_LOG("running_sum_temp= %f len = %u\n\r", running_sum_temp, cbfifo_length(&data_buffer));
   avg_temp = running_sum_temp/cbfifo_length(&data_buffer); 
}

float get_avg_temp()
{
   return avg_temp;
}
