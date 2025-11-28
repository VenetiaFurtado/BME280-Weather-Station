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
 * @file    eled_fsm.c
 * @brief   External LED (ELED) finite state machine implementation.
 *
 * This file implements the FSM controlling the ELED for
 * bus stop indicators. It includes functions to:
 *  - Initialize the FSM (init_eled_fsm())
 *  - Update the FSM periodically (eled_fsm())
 *  - Manage LED states: BLINK, PAUSE, and TRANSITION
 *  - Control LED brightness and station transitions
 *
 * The FSM responds to emergency conditions and tracks timing for blink
 * sequences, pause intervals, and transitions between stations.
 *
 * @author  Venetia Furtado
 * @date    10/06/2025
 *
 */

#include "fsm.h"
#include "log.h"
#include "switch.h"
#include "bme280.h"
#include "data_acquisition.h"

void Init_FSM(FSMInfo *info_ptr)
{
   info_ptr->state = NORMAL;
}

void Handle_FSM(FSMInfo *info_ptr)
{

   //INFO_LOG("get_avg_temp() = %f\n\r", get_avg_temp());
   BME280_Data data;
   acquire_data(&data);

   switch (info_ptr->state)
   {
   case NORMAL:
      /* code */
      INFO_LOG("Read values: Temp %f Pressure %f Humidity %f",
               data.temperature,
               data.pressure,
               data.humidity);

      if (was_switch_activated() == true)
      {
         info_ptr->state = USER;
         INFO_LOG("State Transition: NORMAL -> USER");
      }
      else if (get_avg_temp() >= EMERGENCY_THRESHOLD)
      {
         info_ptr->state = EMERGENCY;
         INFO_LOG("State Transition: NORMAL -> EMERGENCY");
      }


      break;
   case EMERGENCY:
      /* code */
      INFO_LOG("HIGH TEMPERATURE WARNING : %f", get_avg_temp());

      if (was_switch_activated() == true)
      {
         info_ptr->state = USER;
         INFO_LOG("State Transition: EMERGENCY -> USER");
      }
      else if (get_avg_temp() < EMERGENCY_THRESHOLD)
      {
         info_ptr->state = NORMAL;
         INFO_LOG("State Transition: EMERGENCY -> NORMAL");
      }
      break;

   case USER:
      /* code */
      USER_LOG("Average Temperature %f", get_avg_temp());

      if (get_avg_temp() >= EMERGENCY_THRESHOLD)
      {
         info_ptr->state = EMERGENCY;
         INFO_LOG("State Transition: USER -> EMERGENCY");
      }
      else
      {
         info_ptr->state = NORMAL;
         INFO_LOG("State Transition: USER -> NORMAL");
      }

      break;
   }
}
