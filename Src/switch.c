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
 * @file    switch.c
 * @brief   User switch B1 interface and external interrupt handling.
 *
 * This file provides functions to initialize the user switch on PC13,
 * detect its current state, and handle activation events via EXTI interrupts.
 * It includes:
 *  - GPIO and EXTI configuration for the switch.
 *  - EXTI interrupt handler for switch presses.
 *  - Functions to query switch state (is_switch_pressed()) and 
 *    activation events (was_switch_activated()).
 * 
 * @author  Venetia Furtado
 * @date    10/06/2025
 *
 */
#include <stm32f091xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "utilities.h"

#define MASK(x) (1UL << (x))
#define SW1_POS (13) /*PC13 (User Button B1)*/
#define SW_GPIO (GPIOC) /*Both switches are on GPIO C*/
#define SWITCH_PRESSED(x) (!((SW_GPIO)->IDR & MASK(x))) /*Both switches are active-low, so invert data*/
#define ENABLE_PULLUP 1
#define EXTICR4 3
#define EXTIx_CONFIG_BITS_PC 2
#define PRIORITY_LEVEL 2
#define CLEAR_PENDING_REQUEST 0x0000fff0


bool switch_activated = false;

/**
 * @brief Initializes the user switch B1 on PC13 as an external interrupt.
 *
 * This function configures the GPIO and interrupt system for the user switch:
 *  - Enables the peripheral clock for GPIOC.
 *  - Configures PC13 as a digital input with a pull-up resistor.
 *  - Enables the SYSCFG peripheral clock for external interrupt configuration.
 *  - Maps PC13 to EXTI line 13 for external interrupts.
 *  - Configures EXTI to trigger on the falling edge(switch press).
 *  - Sets up the NVIC to handle the EXTI4_15 interrupt with priority 2.
 *  - Clears any pending interrupt flags and enables the IRQ.
 * 
 * @note This function has been referenced from:
 * https://github.com/alexander-g-dean/ESF/tree/master/ST/Code/ch7
 */
void Init_switch()
{
   RCC->AHBENR |= RCC_AHBENR_GPIOCEN;                                    /*Enable peripheral clock of GPIOC (for Switch B1)*/
   MODIFY_FIELD(GPIOC->MODER, GPIO_MODER_MODER13, ESF_GPIO_MODER_INPUT); /*​Configure PC13 in input mode*/
   MODIFY_FIELD(GPIOC->PUPDR, GPIO_PUPDR_PUPDR13, ENABLE_PULLUP);                    /*​Enable pullup on each input with 01*/

   
   RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN; /*Enable peripheral clock for SYSCFG*/

   MODIFY_FIELD(SYSCFG->EXTICR[EXTICR4], SYSCFG_EXTICR4_EXTI13, EXTIx_CONFIG_BITS_PC); /* sets up PC13 as external interrupt line */
   EXTI->IMR |= MASK(SW1_POS);        /*Set mask bits for inputs in EXTI_IMR*/
   EXTI->FTSR |= MASK(SW1_POS);       /*Trigger on falling edges in EXTI_RTSR and EXTI_FTSR*/

   /*Configure enable and mask bits for NVIC IRQ Channel for EXTI*/
   /*Interrupt lines 7 and 13 are both serviced by EXTI4_15_IRQ*/
   NVIC_SetPriority(EXTI4_15_IRQn, PRIORITY_LEVEL);
   NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
   NVIC_EnableIRQ(EXTI4_15_IRQn);

   __enable_irq();
}

/**
 * @brief Interrupt handler for external interrupts on lines 4 to 15.
 * This handler services interrupts for the user switch connected to PC13
 * (EXTI line 13).
 * All other pending EXTI interrupts within this range (lines 4–15) are cleared
 * at the end of the handler.
 * 
 * @note This function has been referenced from:
 * https://github.com/alexander-g-dean/ESF/tree/master/ST/Code/ch7
 */
void EXTI4_15_IRQHandler(void)
{
   if ((EXTI->PR & EXTI_PR_PR13) != 0)
   {
      EXTI->PR |= EXTI_PR_PR13; /*clear pending request*/
      switch_activated = true;
   }
   
   EXTI->PR = CLEAR_PENDING_REQUEST; /*Clear all other pending requests for this handler*/
}

/**
 * @brief Checks if the user switch B1 is currently pressed.
 *
 * @return true if the switch is pressed, false otherwise.
 */
bool is_switch_pressed()
{
   return (SWITCH_PRESSED(SW1_POS));
}

/**
 * @brief Checks if the user switch B1 was activated since the last check.
 *
 * This function returns true if the switch was pressed and the
 * switch_activated flag was set by the EXTI interrupt handler. After
 * returning true, it clears the flag so subsequent calls will return false
 * until the switch is pressed again.
 *
 * @return true if the switch was activated since the last check, false otherwise.
 */
bool was_switch_activated()
{
   bool test = false;
   uint32_t masking_state = __get_PRIMASK();
   __disable_irq();
   if (switch_activated == true)
   {
      switch_activated = false;
      test = true;
   }
   __set_PRIMASK(masking_state);
   return test;
}
