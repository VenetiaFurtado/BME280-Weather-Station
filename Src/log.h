#ifndef _LOG_H_
#define _LOG_H_

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
 * @file    log.h
 * @brief	Logging macros for debugging.
 *
 * This header provides macros for printing log messages
 * to the console when `DEBUG` is defined:
 * 
 *  - INFO_LOG(...)    : Prints informational messages.
 *
 * @author  Venetia Furtado
 * @date    10/06/2025
 *
 */
#include <stdio.h>

#ifdef DEBUG
#define INFO_LOG(...) printf("\n\r");printf(__VA_ARGS__); printf("\n\r")
#else
#define INFO_LOG(...)
#endif

//#define USER_LOG(...) printf("\n\r");printf(__VA_ARGS__); printf("\n\r")
#define STATE_TRANSITION_LOG(...) printf("\n\r\033[1;34m" __VA_ARGS__);printf("\033[0m\n\r") //blue
#define USER_LOG(...) printf("\n\r\033[38;5;214m" __VA_ARGS__);printf("\033[0m\n\r")//orange
#define WARNING_LOG(...) printf("\n\r\033[1;31m" __VA_ARGS__);printf("\033[0m")   //red

#endif
