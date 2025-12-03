#ifndef __SWITCH_H
#define __SWITCH_H
#include <stdbool.h>

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
 * @file    switch.h
 * @brief   Header file for user switch (B1) interface and EXTI interrupt handling.
 * 
 * @author  Venetia Furtado
 * @date    10/06/2025
 *
 */
void Init_switch();
bool is_switch_pressed();
bool was_switch_activated();


#endif