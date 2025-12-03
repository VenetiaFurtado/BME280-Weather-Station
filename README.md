[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/M-vWKnx0)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=21811110&assignment_repo_type=AssignmentRepo)
# Final Project (Fall 2025)
# BME280 Weather Station
## Overview
This project implements an environment monitoring system based on an STM32F091RC 
microcontroller. A BME280 sensor is interfaced through I2C to acquire temperature, 
pressure, and humidity data. The system features a finite-state machine (FSM) - 
using the switch/case approach, that transitions between NORMAL,
EMERGENCY, and USER states based on temperature thresholds and user input.

A circular buffer stores recent temperature samples to maintain a running average. 
LED behavior, including blink rate and brightness is controlled via TIM7 and PWM 
(TIM2) to visually indicate system state. Peripheral drivers were implemented in 
bare-metal C for I2C, SPI, timers, PWM, and GPIO.
**NOTE:** The sensor’s I2C communication did not function correctly at first. 
Due to time constraints, the project was completed using the SPI interface as a 
fallback. I later resolved the I2C read/write issues, but the SPI interface 
produced more reliable measurements, so the project includes code for both interfaces
under `#ifdef` blocks. However, since the initial proposal involved using I2C, 
all test results presented correspond to configurations with I2C interfacing enabled.

## Code structure
This project is structured into several software modules that work together to 
read environmental data from the BME280 sensor, process it, and update the LED 
indicator (LD2) and terminal output based on system state.
`main()` - Serves as the entry point of the firmware is responsible for the 
initialization and execution of all peripherals and system modules. The structure 
is modular, with each function responsible for a specific hardware or software component.
**Communication Interface Initialization**
`I2C_Init()`: Sets up the I2C1 peripheral, configures GPIO pins, timing, and the BME280 interface.

