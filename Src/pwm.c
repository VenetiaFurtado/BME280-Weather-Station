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
//#include "gpio.h"
#include "utilities.h"
//#include "systick.h"

#include "pwm.h"


#define F_TIM_CLOCK (48UL*1000UL*1000UL)	/*48 MHz*/
#define PWM_FREQUENCY (500)
#define PWM_MAX_DUTY_VALUE ( (F_TIM_CLOCK / (PWM_FREQUENCY * PWM_PRESCALER)) - 1)
#define PWM_BRIGHTNESS_INTERVAL (PWM_MAX_DUTY_VALUE / 255)
#define PWM_PRESCALER (2)
#define DIM_DELAY (100)
#define TRANSITION_PERIOD (500)
#define REGISTER_VAL 0
#define ALT_FUNCTION 1
#define PWM_MODE 6
#define DEFAULT_VAL 1

uint8_t current_brightness_level = MAXIMUM_LED_BRIGHTNESS;


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

#if 0
/**
 * @brief Initializes the PWM peripheral for external LED (ELED) brightness control.
 *
 * This function configures Timer 3 Channel 2 (PA7) on the STM32F0 
 * microcontroller to generate a PWM signal used for controlling the 
 * brightness of an external LED. It performs the following steps:
 *
 * - Enables the GPIOA and TIM3 peripheral clocks.
 * - Configures PA7 as an alternate function pin mapped to TIM3_CH2.
 * - Sets up the TIM3 prescaler and auto-reload registers to achieve 
 *   the desired PWM frequency.
 * - Enables the main output and starts the timer.
 * 
 * @note This code has been referenced from:
 * https://github.com/alexander-g-dean/ESF/tree/master/ST/Code/ch7
 */
void init_eled_pwm(void)
{
	/*Configure PA7 with alternate function 1: TIM3_CH2*/
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER7, ESF_GPIO_MODER_ALT_FUNC);
	MODIFY_FIELD(GPIOA->AFR[REGISTER_VAL], GPIO_AFRL_AFRL7, ALT_FUNCTION);

	/*Configure TIM3 counter and prescaler*/
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = PWM_PRESCALER - 1;
	TIM3->ARR = PWM_MAX_DUTY_VALUE;
	TIM3->CR1 = 0; /*count up*/

	/*Configure TIM3 channel 2*/
	TIM3->CCR2 = DEFAULT_VAL;										 /*Short on-time by default*/
	TIM3->CCER |= TIM_CCER_CC2P;						 /*active low polarity*/
	MODIFY_FIELD(TIM3->CCMR1, TIM_CCMR1_OC2M, PWM_MODE); /*Select PWM mode*/
	TIM3->CCMR1 |= TIM_CCMR1_OC2PE;					 /*Enable preload register*/
	TIM3->EGR |= TIM_EGR_UG;							 /*Generate update*/
	TIM3->CCER |= TIM_CCER_CC2E;						 /*Enable channel output on OC2*/
	TIM3->BDTR |= TIM_BDTR_MOE;						 /*Enable main output*/

	TIM3->CR1 |= TIM_CR1_CEN; /*Enable timer*/
}
#endif

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
	TIM2->PSC = PWM_PRESCALER - 1; //47;  // Prescaler: 48MHz / (47+1) = 1MHz
	TIM2->ARR = PWM_MAX_DUTY_VALUE; //999; // Auto-reload: 1MHz / (999+1) = 1kHz PWM frequency

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
