/**
 * @file    buffer.c
 * @brief	This file contains the implementation of circular buffer.
 *
 * @author  Venetia Furtado
 * @date    11/03/2025
 *
 */
#include <stdbool.h>
#include "buffer.h"

#define SUCCESS 1
#define ERROR -1
/**
 * @brief Initializes the head and tail of the circular buffer
 * 
 * @param Address of circular buffer
 */
void init_buffer_with_default_val(BufferType* bufferLog)
{
   bufferLog->head = 0;
   bufferLog->tail = 0;
   bufferLog->length = 0;
}

/**
 * @brief Checks is circular buffer is empty.
 * @param Address of circular buffer 
 * @return true if buffer is empty
 * @return false if buffer not empty
 */
bool is_buffer_empty(BufferType* bufferLog)
{
    return bufferLog->head == bufferLog->tail;
}

/**
 * @brief Checks if buffer is full
 * 
 * @param Address of circular buffer  
 * @return true if buffer full
 * @return false if buffer is not full
 */
bool is_buffer_full(BufferType* bufferLog)
{
   // empty buffer
   if (bufferLog->head == bufferLog->tail)
   {
      return false;
   }

   // head > tail
   if ((bufferLog->head > bufferLog->tail) &&               //Head is greater than tail 
       ((bufferLog->head - bufferLog->tail + 1) == BUFFER_SIZE))   //Head is at the last location and tail is at first location of buffer   
   {
      return true;
   }

   // tail > head
   if ((bufferLog->head < bufferLog->tail) &&               //Tail is greater than head
       ((bufferLog->tail - bufferLog->head - 1) == 0))      //Tail is one location ahead of head
   {
      return true;
   }

   return false;                                //return false for every other condition
}

/**
 * @brief Puts character at the head of buffer
 * 
 * @param Address of circular buffer
 * @param Character to be put into circular buffer
 * @return int 
 */
int write_to_buffer(BufferType* bufferLog, const BME280_Data* c)
{
   if (is_buffer_full(bufferLog) == true)
   {
      return ERROR;
   }
   bufferLog->buffer[bufferLog->head] = *c;             // put value of c into the buffer head
   bufferLog->head++;                                  //increments head by 1

   if (bufferLog->head == BUFFER_SIZE)                       //when head reaches end of buffer, reset it to intial location
   {
      bufferLog->head = 0;
   }
   bufferLog->length++;
   return SUCCESS;
}

/**
 * @brief Gets character from Buffer Tail
 * 
 * @param Address of circular buffer
 * @param Character to be received from the tail
 * @return int 
 */
int read_from_buffer(BufferType* bufferLog, BME280_Data* c)
{
   if (is_buffer_empty(bufferLog) == true)
   {
      return ERROR;
   }

   // put value ofbuffer tail into c
   *c = bufferLog->buffer[bufferLog->tail];
   bufferLog->tail++;

   if (bufferLog->tail == BUFFER_SIZE)
   {
      bufferLog->tail = 0;
   }
   bufferLog->length--;

   return SUCCESS;
}

/**
 * @brief Gets the length of the circular buffer
 * 
 * @return int
 */
inline int cbfifo_length(const BufferType* bufferLog)
{
   return bufferLog->length;
}

