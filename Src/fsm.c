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
 * @file    fsm.c
 * @brief   This file contains the implementation of the finite state machine (FSM)
 * used to manage system states based on BME280 sensor readings and user input.
 * It includes functions to initialize the FSM, run it continuously, update
 * states, control LED behavior, and handle state-dependent events and logging.
 *
 * @author  Venetia Furtado
 * @date    11/02/2025
 *
 */

#include "fsm.h"
#include "log.h"
#include "switch.h"
#include "bme280.h"
#include "data_acquisition.h"
#include "pwm.h"
#include "systick.h"

FSMInfo info;

/**
 * @brief Initializes the FSM.
 * This function sets the FSM to its default starting state `NORMAL`.
 */
void Init_FSM()
{
   info.state = NORMAL;
}

/**
 * @brief Returns the LED blink frequency based on the current FSM state.
 *
 * This function determines the appropriate blink period for the ULED
 * depending on whether the system is in NORMALor EMERGENCY or USER state.
 *
 * @return uint16_t Blink period value (ARR register value) corresponding
 *                  to the current state.
 *
 * @note Uses the global `info.state` to determine the current FSM state.
 */
uint16_t blink_frequency()
{
   uint16_t arr_val = NORMAL_PERIOD;
   if (info.state == NORMAL)
   {
      arr_val = NORMAL_PERIOD;
   }
   else
   {
      arr_val = EMERGENCY_PERIOD;
   }
   return arr_val;
}

/**
 * @brief Updates the LED brightness based on the current FSM state.
 *
 * This function controls the LED behavior according to the system state:
 *  - In the USER state, the LED is set to a fixed USER_BRIGHTNESS.
 *  - In NORMAL or EMERGENCY states, the LED toggles between 0 and the
 *    corresponding state-specific brightness (NORMAL_BRIGHTNESS or
 *    EMERGENCY_BRIGHTNESS) to create a blinking effect.
 *
 * The computed brightness is applied using the `led_brightness()` function.
 */
void blink_LED()
{
   if (info.state == USER)
   {
      info.led_brightness = USER_BRIGHTNESS;
   }
   else
   {
      if (info.led_brightness == 0)
      {
         if (info.state == NORMAL)
         {
            info.led_brightness = NORMAL_BRIGHTNESS;
         }
         else
         {
            info.led_brightness = EMERGENCY_BRIGHTNESS;
         }
      }
      else
      {
         info.led_brightness = 0;
      }
   }
   led_brightness(info.led_brightness);
}

/**
 * @brief Executes one iteration of the FSM
 *
 * This function performs the following actions based on the current system state:
 *  - Reads the latest environmental data from the BME280 sensor.
 *  - Updates the FSM state based on sensor readings and user switch input.
 *  - Logs relevant information depending on the state:
 *      - NORMAL: Logs current temperature, pressure, and humidity.
 *      - EMERGENCY: Logs a high-temperature warning.
 *      - USER: Logs the moving average temperature.
 *  - Handles state transitions with appropriate logging:
 *      - NORMAL -> USER, NORMAL -> EMERGENCY
 *      - EMERGENCY -> USER, EMERGENCY -> NORMAL
 *      - USER -> NORMAL, USER -> EMERGENCY
 */
void FSM()
{
   BME280_Data data;
   acquire_data(&data);

   switch (info.state)
   {
   case NORMAL:
      INFO_LOG("Read values: Temp %0.2f°C Pressure %0.2fhPa Humidity %0.2f%%",
               data.temperature,
               data.pressure,
               data.humidity);

      if (was_switch_activated() == true)
      {
         info.state = USER;
         STATE_TRANSITION_LOG("State Transition: NORMAL -> USER");
      }
      else if (data.temperature >= EMERGENCY_THRESHOLD)
      {
         info.state = EMERGENCY;
         STATE_TRANSITION_LOG("State Transition: NORMAL -> EMERGENCY");
      }
      break;
   case EMERGENCY:
      WARNING_LOG("HIGH TEMPERATURE WARNING : %0.2f°C", data.temperature);
      if (was_switch_activated() == true)
      {
         info.state = USER;
         STATE_TRANSITION_LOG("State Transition: EMERGENCY -> USER");
      }
      else if (data.temperature < EMERGENCY_THRESHOLD)
      {
         info.state = NORMAL;
         STATE_TRANSITION_LOG("State Transition: EMERGENCY -> NORMAL");
      }
      break;

   case USER:
      USER_LOG("Average Temperature = %0.2f°C", get_avg_temp());

      if (data.temperature >= EMERGENCY_THRESHOLD)
      {
         info.state = EMERGENCY;
         STATE_TRANSITION_LOG("State Transition: USER -> EMERGENCY");
      }
      else
      {
         info.state = NORMAL;
         STATE_TRANSITION_LOG("State Transition: USER -> NORMAL");
      }
      break;
   }
}

/**
 * @brief This function implements the main FSM execution loop. It repeatedly
 * checks the system tick counter (from SysTick) and calls `FSM()`
 * whenever a new tick is detected. This ensures the FSM runs at a
 * fixed interval determined by the SysTick timer.
 *
 * @note This function never returns and is intended to be the main
 *       control loop of the application.
 * @note Relies on get_current_tick() to track SysTick increments.
 */
void run_FSM()
{
   ticktime_t tick_counter = 0;
   while (1)
   {
      if (tick_counter != get_current_tick())
      {
         FSM();
         tick_counter = get_current_tick();
      }
   }
}
