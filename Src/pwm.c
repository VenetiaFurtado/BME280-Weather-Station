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
 * @brief 
 *
 * @author  Venetia Furtado
 * @date    12/02/2025
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
#define PWM_MODE_1 (6U << 4)

uint8_t current_brightness_level = MAXIMUM_LED_BRIGHTNESS;

/**
 * @brief Initializes PWM output on PA5 using TIM2 Channel 1.
 *
 * This function configures GPIOA pin 5 as an alternate function for TIM2_CH1
 * and sets up TIM2 to generate a PWM signal with a 1 kHz frequency. The PWM
 * channel is configured in PWM mode 1 with preload enabled, and the timer
 * is started immediately.
 *
 * Configuration details:
 *  - System clock assumed: 48 MHz
 *  - Timer prescaler: PWM_PRESCALER
 *  - Auto-reload value: PWM_MAX_DUTY_VALUE
 *  - PWM frequency: 1 kHz
 *  - Initial duty cycle: 50%
 * 
 * Source: https://github.com/alexander-g-dean/ESF/blob/master/ST/Code/ch7/PWM/main.c
 */
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
	TIM2->CCMR1 |= PWM_MODE_1;		  // PWM mode 1 (0b110)
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE; // Enable preload register
	TIM2->CCR1 = 1; // 50% duty cycle (500/1000)

	// Enable channel 1 output
	TIM2->CCER |= TIM_CCER_CC1E;

	// Enable timer
	TIM2->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief Sets the brightness of the ULED using PWM.
 *
 * @param brightness_level Desired brightness level (0 = off, FF = maximum brightness).
 */
void led_brightness(uint8_t brightness_level)
{
	uint32_t calculated_brightness_level = brightness_level * PWM_BRIGHTNESS_INTERVAL;
	TIM2->CCR1 = calculated_brightness_level;
	current_brightness_level = brightness_level;
}
