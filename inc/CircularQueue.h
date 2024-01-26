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
    void** circular_buffer;
    uint8_t* occupied;
    uint8_t read_index;
    uint8_t write_index;
    uint8_t size;
    uint8_t max_size;
    size_t data_size;
    pthread_mutex_t* mutex;
} circular_queue_handle;

/**
 * @brief Initialize a circular queue
 *
 * @param handle The handle to the queue
 * @param data_size The size of the data type
 * @param length The maxinum length of the queue
 */
void queue_init(circular_queue_handle* handle, size_t data_size, uint8_t length);

/**
 * @brief Destroy the given queue
 *
 * @param handle The handle of the queue
 */
void queue_destroy(circular_queue_handle* handle);

/**
 * @brief Enqueue a message to the end of the given queue
 *
 * @param handle The handle of the queue
 * @param data The data to add to the queue
 */
void queue_get_next_open(circular_queue_handle* handle, void* data);

/**
 * @brief Dequeue a given message from the queue
 *
 * @param handle The handle of the queue
 * @param data The data to remove from the queue
 */
void queue_deqeueue(circular_queue_handle* handle, void* data);

uint8_t find_next_open(circular_queue_handle* handle);
