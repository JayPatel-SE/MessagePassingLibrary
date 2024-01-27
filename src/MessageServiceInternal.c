/**
 * @file MessageServiceInternal.c
 * @author Jay Patel
 * @brief The implementation of the internal Message Service library
 * @version 0.1
 * @date 2024-01-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "MessageServiceInternal.h"

#include <stdio.h>
#include <stdlib.h>

// A queue of all of the messages sent ready to be received
static buffer_pool_handle_t* packets;
// A queue of available messages
static buffer_pool_handle_t* messages;

// A counter to assign unique identifier to new threads
static new_user_id_t* id_gen;

error_t internal_init(system_conf_t* conf)
{
    buffer_pool_init(messages, sizeof(message_t), conf->max_messages);
    buffer_pool_init(packets, sizeof(packet_t), conf->max_packets);

    for (int i = 0; i < conf->max_packets; i++)
    {
        packet_t* temp = (packet_t*)packets->circular_buffer[i];
        temp->dst = -1;
    }

    return kOk;
}

error_t get_new_message(message_t* msg)
{
    if (msg == NULL)
    {
        printf("ERROR: Given null message struct\n");
        return kErrParam;
    }

    pthread_mutex_lock(messages->mutex);
    if (messages->size == messages->max_size)
    {
        pthread_mutex_unlock(messages->mutex);
        printf("ERROR: No more messages available\n");
        return kErr;
    }

    for (int i = 0; i < messages->max_size; i++)
    {
        if (messages->occupied[i] == 0)
        {
            messages->size++;
            messages->occupied[i] = 1;
            msg = (message_t*)messages->circular_buffer[i];
            printf("INFO: Get new message from buffer, p:%p, i:%d\n", msg, i);
            return kOk;
        }
    }
    pthread_mutex_unlock(messages->mutex);

    printf("ERROR: No new messages left in buffer pool\n");
    return kErr;
}

error_t return_used_message(message_t* msg)
{
    if (msg == NULL)
    {
        printf("ERROR: Given null message struct\n");
        return kErrParam;
    }

    pthread_mutex_lock(messages->mutex);
    for (int i = 0; i < messages->max_size; i++)
    {
        if (messages->circular_buffer[i] == msg)
        {
            messages->size--;
            messages->occupied[i] = 0;
            printf("INFO: Returned message back to buffer, p:%p, i:%d\n", msg, i);
            return kOk;
        }
    }
    pthread_mutex_unlock(messages->mutex);

    printf("ERROR: Given msg was not found, %p\n", msg);
    return kErr;
}

error_t send_packet(uint8_t destination_id, message_t* msg)
{
    if (msg == NULL)
    {
        printf("ERROR: given message is NULL\n");
        return kErrParam;
    }

    pthread_mutex_lock(packets->mutex);
    if (packets->size == packets->max_size)
    {
        pthread_mutex_unlock(packets->mutex);
        printf("ERROR: No more packets available\n");
        return kErr;
    }

    for (int i = 0; i < packets->max_size; i++)
    {
        if (packets->occupied[i] == 0)
        {
            packets->size++;
            packets->occupied[i] = 1;
            packet_t* packet = (packet_t*)messages->circular_buffer[i];
            packet->dst = destination_id;
            packet->msg = msg;
            printf("INFO: Get new packet from buffer, p:%p, i:%d\n", packet, i);
            return kOk;
        }
    }

    pthread_mutex_unlock(packets->mutex);

    printf("ERROR: Given msg was not sent\n");
    return kErr;
}

error_t receive_packet(uint8_t receiver_id, message_t* msg)
{
    pthread_mutex_lock(packets->mutex);

    for (int i = 0; i < packets->max_size; i++)
    {
        packet_t* pkt = (packet_t*)packets->circular_buffer[i];
        if (pkt->dst == receiver_id)
        {
            msg = pkt->msg;
            packets->occupied[i] = 0;
            packets->size--;
            printf("INFO: Packet received, releasing..., p:%p, i:%d\n", pkt, i);
            return kOk;
        }
    }
    pthread_mutex_unlock(packets->mutex);

    printf("ERRORR: No packet was received, rec_id=%d\n", receiver_id);
    return kErr;
}

void buffer_pool_init(buffer_pool_handle_t* handle, size_t data_size, uint8_t length)
{
    if (handle == NULL)
    {
        printf("ERROR: The given handle is null\n");
        return;
    }

    handle = malloc(sizeof(buffer_pool_handle_t));
    handle->circular_buffer = malloc(sizeof(void*) * length);
    handle->occupied = malloc(sizeof(uint8_t) * length);
    handle->max_size = length;
    pthread_mutex_init(handle->mutex, NULL);

    for (int i = 0; i < length; i++)
    {
        handle->circular_buffer[i] = malloc(data_size);
        handle->occupied[i] = 0;
    }
}

void buffer_pool_destory(buffer_pool_handle_t* handle)
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
