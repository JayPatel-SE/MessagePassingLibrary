/**
 * @file CircularQueue.c
 * @author Jay Patel
 * @brief CiruclarQueue Implementation
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "CircularQueue.h"

void queue_init(circular_queue_handle* handle, size_t data_size, uint8_t length)
{
    if (handle == NULL)
    {
        printf("ERROR: The given handle is null\n");
        return;
    }

    handle = malloc(sizeof(circular_queue_handle));
    handle->circular_buffer = malloc(sizeof(void*) * length);
    handle->occupied = malloc(sizeof(uint8_t) * length);
    handle->read_index = 0;
    handle->write_index = 0;
    handle->max_size = length;
    handle->data_size = data_size;
    pthread_mutex_init(handle->mutex, NULL);

    for (int i = 0; i < length; i++)
    {
        handle->circular_buffer[i] = malloc(data_size);
        handle->occupied[i] = 0;
    }
}

void queue_destroy(circular_queue_handle* handle)
{
    if (handle == NULL)
    {
        printf("ERROR: The given handle is null\n");
        return;
    }

    pthread_mutex_destroy(handle->mutex);
    free(handle->circular_buffer);
    free(handle);
}

void queue_get_next_open(circular_queue_handle* handle, void* data)
{
    if (handle == NULL)
    {
        printf("ERROR: The given handle is null\n");
        return;
    }

    if (data == NULL)
    {
        printf("ERROR: The data is null\n");
        return;
    }

    pthread_mutex_lock(handle->mutex);

    if (handle->size == handle->max_size)
    {
        pthread_mutex_unlock(handle->mutex);
        printf("ERROR: The buffer is full, cannot add data\n");
        return;
    }

    data = handle->circular_buffer[handle->write_index];
    handle->occupied[handle->write_index] = 1;
    handle->size++;
    handle->write_index = find_next_open(handle);
    pthread_mutex_unlock(handle->mutex);
}

void queue_deqeueue(circular_queue_handle* handle, void* data)
{
    if (handle == NULL)
    {
        printf("ERROR: The given handle is null\n");
        return;
    }

    if (data == NULL)
    {
        printf("ERROR: The data is null\n");
        return;
    }

    if (handle->size == handle->max_size)
    {
        printf("ERROR: The buffer is full, cannot add data\n");
        return;
    }
}

uint8_t find_next_open(circular_queue_handle* handle)
{
    if (handle == NULL)
    {
        printf("ERROR: The given handle is null\n");
        return;
    }

    uint8_t index = handle->write_index;
    while(handle->occupied[index] != 0)
    {
        index++;
        index = index % handle->max_size;
    }

    return index;
}