#ifndef __PWM_H
#define __PWM_H

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
 * @file    pwm.h
 * @brief   Header file for PWM-based LED control.
 * @author  Venetia Furtado
 * @date    10/06/2025
 *
 */

#define MINIMUM_LED_BRIGHTNESS 0
#define MAXIMUM_LED_BRIGHTNESS 0xFF

void led_brightness(uint8_t brightness_level);
void init_eled_pwm(void);

#endif