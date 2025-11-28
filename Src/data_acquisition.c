#include <stm32f091xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "data_acquisition.h"
#include "bme280.h"
#include "buffer.h"
#include "utilities.h"
#include "log.h"

#define TIM7_PSC_VAL 47999
#define TIM7_ARR_VAL 999

BufferType data_buffer;
double running_sum_temp;
float avg_temp;

void Init_DataAcquisition()
{
   init_buffer_with_default_val(&data_buffer);
   Init_TIM7();
}

/**
 * @brief Initializes TIM7.
 * 
 * * Reference:
 * 1. RM0091 Reference manual -  Chapter 21(TIM6/TIM7)
 * 2. RM0091 Reference manual - Appendix code examples
 */
void Init_TIM7(void)
{
   // Enable TIM7 clock
   RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

   // Disable timer during configuration
   TIM7->CR1 = 0;

   // Configure prescaler and auto-reload for 3 seconds
   // TIM7 clock = 48 MHz
   // PSC = 47999 --> 48MHz / (47999 + 1) = 1 kHz (1 ms per tick)
   // ARR = 999 --> 1000 ticks at 1 kHz = 1 seconds
   TIM7->PSC = TIM7_PSC_VAL;
   TIM7->ARR = TIM7_ARR_VAL;

   // Enable update interrupt
   TIM7->DIER |= TIM_DIER_UIE;

   // Configure NVIC for TIM7
   NVIC_SetPriority(TIM7_IRQn, 2);
   NVIC_ClearPendingIRQ(TIM7_IRQn);
   NVIC_EnableIRQ(TIM7_IRQn);

   // Generate update event to load PSC and ARR
   TIM7->EGR = TIM_EGR_UG;

   // Clear the update flag
   TIM7->SR &= ~TIM_SR_UIF;

   // Enable and start the timer
   TIM7->CR1 |= TIM_CR1_CEN;
}

void acquire_data()
{
   BME280_Data data;

   BME280_ReadAll(&data);

   INFO_LOG("Current Temp = %f\n\r", data.temperature);

   if (write_to_buffer(&data_buffer, &data) == -1)
   {
      INFO_LOG("Write to buffer failed!!");
   }

   running_sum_temp += data.temperature;

   if(cbfifo_length(&data_buffer) == 30)
   {
      INFO_LOG("Reading tail from buffer");
      BME280_Data old_sample;
      read_from_buffer(&data_buffer, &old_sample);
      running_sum_temp -= old_sample.temperature;
   }

   INFO_LOG("running_sum_temp= %f len = %u\n\r", running_sum_temp, cbfifo_length(&data_buffer));
   avg_temp = running_sum_temp/cbfifo_length(&data_buffer); 
}

float get_avg_temp()
{
   return avg_temp;
}

/**
 * @brief TIM7 interrupt handler-fires every 3 seconds
 */
void TIM7_IRQHandler(void)
{
   // Check if update interrupt flag is set
   if (TIM7->SR & TIM_SR_UIF)
   {
      TIM7->SR &= ~TIM_SR_UIF; // Clear interrupt flag
      acquire_data();
   }
}