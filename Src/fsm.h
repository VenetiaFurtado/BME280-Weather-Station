#ifndef __FSM_H
#define __FSM_H
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
 * @file    fsm.h
 * @brief   Header file for the FSM.
 *
 * This header defines the data structures, enumerations, and function
 * prototypes for controlling the ELED behavior. The FSM manages
 * LED blinking, pause, and transition states, including station-based
 * brightness levels and emergency handling.
 * 
 * @author  Venetia Furtado
 * @date    10/06/2025
 *
 */
#include <stdbool.h>
#include <stdint.h>

#define EMERGENCY_THRESHOLD 24

typedef enum
{
   NORMAL_BRIGHTNESS = 0x87,
   EMERGENCY_BRIGHTNESS  = 0xFF,
   USER_BRIGHTNESS  = 0x0F
} LEDBrightness;

typedef enum
{
   NORMAL,
   EMERGENCY,
   USER
} FSMState;

typedef enum
{
   NORMAL_PERIOD = 999, //1 sec
   EMERGENCY_PERIOD = 249 //0.25 sec
}LEDPeriod;

typedef struct
{
   FSMState state;
   uint8_t led_brightness;
} FSMInfo;

void Init_FSM();
void FSM();
uint16_t blink_frequency();
void blink_LED();

#endif