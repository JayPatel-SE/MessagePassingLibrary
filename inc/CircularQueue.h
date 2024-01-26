/**
 * @file CircularQueue.h
 * @author Jay Patel
 * @brief A circular queue implementation
 * @version 0.1
 * @date 2024-01-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "Types.h"

#include <pthread.h>
#include <stdint.h>

typedef struct
{
    message_t* circular_buffer;
    uint8_t read_index;
    uint8_t writeIndex;
    uint8_t size;
    uint8_t max_size;
    pthread_mutex_t mutex;
} circular_queue_handle;

/**
 * @brief Initialize a circular queue
 *
 * @param handle The handle to the queue
 * @param size The maxinum length of the queue
 */
void queue_init(circular_queue_handle* handle, uint8_t size);

/**
 * @brief Enqueue a message to the end of the given queue
 *
 * @param handle The handle of the queue
 * @param msg The message to add to the queue
 */
void queue_enqueue(circular_queue_handle* handle, message_t* msg);

/**
 * @brief Dequeue a given message from the queue
 *
 * @param handle The handle of the queue
 * @param msg The message to remove from the queue
 */
void queue_deqeueue(circular_queue_handle* handle, message_t* msg);
