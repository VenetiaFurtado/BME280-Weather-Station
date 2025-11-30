#include <stm32f091xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "utilities.h"

#if 0
#define TIM7_PSC_VAL 47999
#define TIM7_ARR_VAL 2999

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
   // ARR = 2999 --> 3000 ticks at 1 kHz = 3 seconds
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

/**
 * @brief TIM7 interrupt handler-fires every 3 seconds
 */
void TIM7_IRQHandler(void)
{
   // Check if update interrupt flag is set
   if (TIM7->SR & TIM_SR_UIF)
   {
      TIM7->SR &= ~TIM_SR_UIF; // Clear interrupt flag
   }
}
   #endif