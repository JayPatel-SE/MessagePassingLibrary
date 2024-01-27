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

#include "CircularQueue.h"
#include "MessageServiceInternal.h"

// A queue of all of the messages sent ready to be received
static circular_queue_handle* packets;
// A queue of available messages
static circular_queue_handle* messages;

// A counter to assign unique identifier to new threads
static new_user_id_t* id_gen;

error_t internal_init(system_conf_t* conf)
{
    queue_init(messages, sizeof(message_t), conf->max_messages);
    queue_init(packets, sizeof(packet_t), conf->max_packets);

    for (int i = 0; i < conf->max_packets; i++)
    {
        packet_t* temp = (packet_t*)packets->circular_buffer[i];
        temp->dst = -1;
    }
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