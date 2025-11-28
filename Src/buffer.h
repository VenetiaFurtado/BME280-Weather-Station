#ifndef __BUFFER_H__
#define __BUFFER_H__
/**
 * @file    buffer.h
 * @brief	This file contains the forward declarations of the functions of the 
 *          circular buffer operations.
 *
 * @author  Venetia Furtado
 * @date    11/03/2025
 *
 */
#include <stdint.h>
#include "bme280.h"

#define BUFFER_SIZE 60              // Size for circular buffer

/**
 * @brief Contains the components of the circular buffer
 * 
 */
typedef struct
{
   BME280_Data buffer[BUFFER_SIZE];
   uint16_t head; // producer
   uint16_t tail; // consumer
   uint32_t length;
} BufferType;

/**
 * @brief Initializes the head and tail of the circular buffer
 * 
 * @param Address of circular buffer
 */
void init_buffer_with_default_val(BufferType* bufferLog);
/**
 * @brief Puts character at the head of buffer
 * 
 * @param Address of circular buffer
 * @param Character to be put into circular buffer
 * @return int 
 */
int write_to_buffer(BufferType* bufferLog, const BME280_Data* c);
/**
 * @brief Gets character from Buffer Tail
 * 
 * @param Address of circular buffer
 * @param Character to be received from the tail
 * @return int 
 */
int read_from_buffer(BufferType* bufferLog, BME280_Data* c);

/**
 * @brief Gets the length of the circular buffer
 * 
 * @return int
 */
int cbfifo_length(const BufferType* bufferLog);

#endif
