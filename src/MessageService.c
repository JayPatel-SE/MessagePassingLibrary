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

#include "internal/MessageServiceInternal.h"
#include "utility/log.h"

#include <stdlib.h>
#include <string.h>

error_t message_system_init(system_conf_t* conf)
{
    if (conf == NULL)
    {
        LOGE("Given null system config\n");
        return kErr;
    }

    error_t err = internal_init(conf);
    if (err != kOk)
    {
        LOGE("Unable to initialize the internal message system\n");
        return kErr;
    }
    return kOk;
}

error_t message_system_term()
{
    error_t err = internal_term();
    if (err != kOk)
    {
        LOGE("Unable to terminate the internal message system\n");
        return kErr;
    }
    return kOk;
}

int register_user()
{
    uint8_t new_id;
    error_t err = register_new_user(&new_id);
    if (err != kOk)
    {
        LOGE("failed to register new user, err=%d\n", err);
        return -1;
    }

    return new_id;
}

int remove_user(int user_id)
{
    error_t err = deregister_user(user_id);
    if (err != kOk)
    {
        LOGE("failed to deregister given user=%d, err=%d\n", user_id, err);
        return -1;
    }

    return 0;
}

message_t* new_message(void)
{
    message_t* msg = NULL;
    error_t err = get_new_message(&msg);
    if (err != kOk)
    {
        LOGE("failed to get new message struct\n");
        return NULL;
    }

    return msg;
}

void delete_message(message_t* msg)
{
    if (msg == NULL)
    {
        LOGE("given message is NULL\n");
        return;
    }

    error_t err = return_used_message(msg);
    if (err != kOk)
    {
        LOGE("delete message, given err=%d\n", err);
        return;
    }
}

int send(uint8_t destination_id, message_t* msg)
{
    if (msg == NULL)
    {
        LOGE("given message is NULL\n");
        return -1;
    }

    error_t err = send_packet(destination_id, msg);
    if (err != kOk)
    {
        LOGE("send failed, given err=%d\n", err);
        return -1;
    }

    return 0;
}

int recv(uint8_t receiver_id, message_t* msg)
{
    message_t* msg_temp;
    error_t err = receive_packet(receiver_id, &msg_temp);
    if (err != kOk)
    {
        LOGE("receive failed, returned error=%d\n", err);
        return -1;
    }

    memcpy(msg->data, msg_temp->data, msg_temp->len);
    msg->len = msg_temp->len;

    return 0;
}

int check_occupied(message_t* msg)
{
    if (msg == NULL)
    {
        LOGE("Given msg is NULL\n");
        return -1;
    }

    return check_internal_occupied(msg);
}
