#ifndef __SYSTICK_H
#define __SYSTICK_H

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
 * @file    systick.h
 * @brief   Header file for SysTick timer configuration and timekeeping.
 * 
 * @author  Venetia Furtado
 * @date    12/02/2025
 *
 */
#include <stdint.h>


typedef uint32_t ticktime_t;

void init_systick(void);
void SysTick_Handler(void);
void reset_timer();
ticktime_t get_current_tick();
uint32_t time_since_startup();

#endif
