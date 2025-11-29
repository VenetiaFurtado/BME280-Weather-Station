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
 * @file    pwm.c
 * @brief PWM control module for LED brightness adjustment on STM32F091RC.
 *
 * This file contains functions and macros to configure and control
 * the PWM signal used for driving an external LED (ELED).
 * It sets up Timer 3, Channel 2 (PA7) to generate a PWM output signal
 * with adjustable duty cycle corresponding to LED brightness levels.
 *
 * The PWM frequency and brightness steps are derived from the system clock
 * and prescaler configuration. The module provides functions to:
 *  - Initialize the PWM peripheral.
 *  - Adjust the LED brightness via duty cycle updates.
 *  - Retrieve the current brightness level.
 *
 * @author  Venetia Furtado
 * @date    10/06/2025
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stm32f091xc.h>
#include "utilities.h"
#include "pwm.h"

#define F_TIM_CLOCK (48UL*1000UL*1000UL)	/*48 MHz*/
#define PWM_FREQUENCY (500)
#define PWM_MAX_DUTY_VALUE ( (F_TIM_CLOCK / (PWM_FREQUENCY * PWM_PRESCALER)) - 1)
#define PWM_BRIGHTNESS_INTERVAL (PWM_MAX_DUTY_VALUE / 255)
#define PWM_PRESCALER (2)

uint8_t current_brightness_level = MAXIMUM_LED_BRIGHTNESS;

void PWM_Init(void)
{
	// Enable clocks for GPIOA and TIM2
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// Configure PA5 as alternate function (TIM2_CH1)
	GPIOA->MODER &= ~(3U << (5 * 2));	// Clear mode bits
	GPIOA->MODER |= (2U << (5 * 2));		// Set to alternate function mode
	GPIOA->AFR[0] &= ~(0xF << (5 * 4)); // Clear alternate function bits
	GPIOA->AFR[0] |= (2U << (5 * 4));	// AF2 for TIM2_CH1

	// Configure TIM2
	// Assuming system clock = 48 MHz
	TIM2->PSC = PWM_PRESCALER - 1;   // Prescaler: 48MHz / (47+1) = 1MHz
	TIM2->ARR = PWM_MAX_DUTY_VALUE;  // Auto-reload: 1MHz / (999+1) = 1kHz PWM frequency

	// Configure Channel 1 in PWM mode 1
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M; // Clear output compare mode bits
	TIM2->CCMR1 |= (6U << 4);		  // PWM mode 1 (0b110)
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE; // Enable preload register
	TIM2->CCR1 = 1; // 50% duty cycle (500/1000)

	// Enable channel 1 output
	TIM2->CCER |= TIM_CCER_CC1E;

	// Enable timer
	TIM2->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief Sets the brightness of the ELED using PWM.
 *
 * This function calculates the appropriate PWM duty cycle based on the
 * provided 8-bit brightness level (0–255) and updates TIM3 Channel 2
 * to adjust the LED brightness. It also updates the global variable
 * current_brightness_level.
 *
 * @param brightness_level Desired brightness level (0 = off, 255 = maximum brightness).
 */
void led_brightness(uint8_t brightness_level)
{
	uint32_t calculated_brightness_level = brightness_level * PWM_BRIGHTNESS_INTERVAL;
	TIM2->CCR1 = calculated_brightness_level;
	current_brightness_level = brightness_level;
}
