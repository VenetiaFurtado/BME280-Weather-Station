#ifndef __DATA_ACQUISITION_H
#define __DATA_ACQUISITION_H
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

void Init_TIM7(void);
void Init_DataAcquisition();
float get_avg_temp();

 #endif