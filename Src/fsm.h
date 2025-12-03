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
 * @brief   This header declares functions, macros, and types used to implement
 * the FSM that manages system states based on BME280 sensor readings
 * and user switch input. It provides initialization, state execution,
 * LED control, and continuous FSM running routines.
 *
 * @author  Venetia Furtado
 * @date    11/02/2025
 *
 */
#include <stdbool.h>
#include <stdint.h>

#define EMERGENCY_THRESHOLD 24

/**
 * @brief Defines predefined brightness levels for the ULED(LD2).
 *
 * This enum provides symbolic names for ULED brightness levels
 * corresponding to different system states:
 *  - `NORMAL_BRIGHTNESS` : Brightness level used in NORMAL state.
 *  - `EMERGENCY_BRIGHTNESS` : Maximum brightness used in EMERGENCY state.
 *  - `USER_BRIGHTNESS` : User-defined brightness used in USER state.
 */
typedef enum
{
   NORMAL_BRIGHTNESS = 0x87,
   EMERGENCY_BRIGHTNESS  = 0xFF,
   USER_BRIGHTNESS  = 0x0F
} LEDBrightness;

/**
 * @brief Defines the possible states of the FSM.
 *
 * The FSM manages system behavior based on sensor readings and user input:
 *  - `NORMAL`    : Standard operation state with periodic monitoring.
 *  - `EMERGENCY` : Triggered when sensor readings exceed thresholds.
 *  - `USER`      : Activated by user input for manual control or display.
 */
typedef enum
{
   NORMAL,
   EMERGENCY,
   USER
} FSMState;

/**
 * @brief Defines the LED blink periods corresponding to FSM states.
 *
 * This enumeration provides symbolic names for the LED update intervals:
 *  - `NORMAL_PERIOD`    : Blink period for NORMAL state (1 second).
 *  - `EMERGENCY_PERIOD` : Blink period for EMERGENCY state (0.25 seconds).
 *
 * The values represent timer auto-reload (ARR) register counts.
 */
typedef enum
{
   NORMAL_PERIOD = 999, //1 sec
   EMERGENCY_PERIOD = 249 //0.25 sec
}LEDPeriod;

/**
 * @struct FSMInfo
 * @brief Stores the current state and LED brightness of the FSM.
 *
 * This structure holds runtime information for the FSM:
 *  - `state`          : Current FSM state, defined by `FSMState`
 *  - `led_brightness` : Current LED brightness level, used for LED control
 */
typedef struct
{
   FSMState state;
   uint8_t led_brightness;
} FSMInfo;

void Init_FSM();
void FSM();
uint16_t blink_frequency();
void blink_LED();
void run_FSM();

#endif