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
#include "Types.h"

error_t internal_init(system_conf_t* conf)
{
    queue_init(messages, sizeof(message_t), conf->max_messages);
    queue_init(packets, sizeof(packet_t), conf->max_packets);
    message_use = malloc(sizeof(uint8_t) * conf->max_messages);

    for (int i = 0; i < conf->max_messages; i++)
    {
        message_use[i] = 0;
    }

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

    void * data = NULL;
    queue_get_next_open(messages, (void*)data);
    if (data == NULL)
    {
        printf("ERROR: Could not get new message, NULL returned\n");
        msg = NULL;
        return kErr;
    }
    msg = (message_t*) data;
}

error_t send_packet(uint8_t destination_id, message_t* msg)
{
    if (msg == NULL)
    {
        printf("ERROR: given message is NULL\n");
        return kErrParam;
    }

    void * data = NULL;
    queue_get_next_open(packets, data);
    if (data == NULL)
    {
        printf("ERROR: Could not send message, NULL returned\n");
        return kErr;
    }

    packet_t* packet = (packet_t*) data;
    packet->dst = destination_id;
    packet->msg = msg;
}