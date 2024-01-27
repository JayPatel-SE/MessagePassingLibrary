/**
 * @file MessageService.c
 * @author Jay Patel
 * @brief Implmentation of the Message Service API
 * @version 0.1
 * @date 2024-01-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "MessageService.h"
#include "MessageServiceInternal.h"

error_t message_system_init(system_conf_t* conf)
{
    error_t err = internal_init(conf);
    if (err != kOk)
    {
        printf("ERROR: Unable to initialize the internal message system");
        return kOk;
    }
    return kErr;
}

message_t* new_message(void)
{
    message_t* msg;
    error_t err = get_new_message(msg);
    if (err != kOk)
    {
        printf("ERROR: failed to get new message struct\n");
        return NULL;
    }

    return msg;
}

void delete_message(message_t* msg)
{
    if (msg == NULL)
    {
        printf("ERROR: given message is NULL\n");
        return;
    }

    error_t err = return_used_message(msg);
    if (err != kOk)
    {
        printf("ERROR: delete message, given err=%d\n", err);
        return;
    }
}

int send(uint8_t destination_id, message_t* msg)
{
    if (msg == NULL)
    {
        printf("ERROR: given message is NULL\n");
        return -1;
    }

    error_t err = send_packet(destination_id, msg);
    if (err != kOk)
    {
        printf("ERROR: send failed, given err=%d\n", err);
        return -1;
    }

    return 0;
}

int recv(uint8_t receiver_id, message_t* msg)
{
    if (msg == NULL)
    {
        printf("ERROR: given message is NULL\n");
        return -1;
    }

    error_t err = receive_packet(receiver_id, msg);
    if (err != kOk)
    {
        printf("ERROR: receive failed, returned error=%d\n", err);
        return -1;
    }

    return 0;
}

int register_new_user(void)
{
    return 0;
}