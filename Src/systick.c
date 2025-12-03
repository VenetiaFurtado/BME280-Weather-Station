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
 * @file    systick.c
 * @brief   SysTick timer configuration and timekeeping functions.
 * 
 * This file provides functions to initialize the SysTick timer, handle
 * its interrupts, and manage a global time counter (time_var).
 * 
 * @author  Venetia Furtado
 * @date    12/02/2025
 *
 */
#include <stdio.h>
#include <stm32f091xc.h>
#include "core_cm0.h"
#include "systick.h"

#define F_SYS_CLK (48000000L)
#define DIVISION_FACTOR (8)
#define INTERRUPT_PRIORITY 3
#define MS 1000
#define INTERVAL_MS 1000
#define INTERVAL (MS / INTERVAL_MS)

extern void Set_Clocks_To_48MHz();
ticktime_t time_var = 0;

/**
 * @brief Initializes the SysTick timer to generate periodic interrupts.
 *
 * This function sets up the SysTick timer to generate interrupts at a
 * fixed interval (100 ms) based on the system clock. It performs the following steps:
 *  - Configures the system clock to 48 MHz using Set_Clocks_To_48MHz().
 *  - Sets the SysTick reload value to generate interrupts at the desired interval.
 *  - Sets the SysTick interrupt priority in the NVIC.
 *  - Clears the current SysTick counter value.
 *  - Enables the SysTick timer and its interrupt.
 * 
 * @note This function has been reference from:
 * https://github.com/alexander-g-dean/ESF/tree/master/ST/Code/ch7
 */
void init_systick(void)
{
	SysTick->LOAD = (F_SYS_CLK / (DIVISION_FACTOR * INTERVAL)) - 1; /*Set reload to get 50ms interrupts*/
	NVIC_SetPriority(SysTick_IRQn, INTERRUPT_PRIORITY); /*Set interrupt priority*/
	SysTick->VAL = 0; /*Force load of reload value*/
	SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; /*Enable interrupt, enable SysTick timer*/
   SystemCoreClockUpdate();
}

/**
 * @brief SysTick interrupt handler.
 *
 * This function is called automatically when the SysTick timer reaches zero.
 * It increments the global variable time_var, which has been used for
 * timing purposes such as delay and keeping time count since system startup.
 */
void SysTick_Handler(void)
{
   time_var++;
}

/**
 * @brief Resets the SysTick timer counter.
 * 
 */
void reset_timer()
{
  time_var = 0;
}

/**
 * @brief Retrieves the current value of the SysTick timer counter.
 *
 */
ticktime_t get_current_tick()
{
   return time_var;
}

/**
 * @brief Returns the elapsed time since system startup in milliseconds.
 *
 * @return uint32_t Elapsed time since startup (in ms).
 */
uint32_t time_since_startup()
{
	uint32_t time_now = time_var * INTERVAL_MS;
	return time_now;
}
