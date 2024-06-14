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

#include "internal/MessageServiceInternal.h"
#include "utility/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A queue of all of the messages sent ready to be received
static buffer_pool_handle_t* packets;
// A queue of available messages
static buffer_pool_handle_t* messages;
// Linked list of all the of the registered users
static linked_list_handle_t* registered_users;

error_t internal_init(system_conf_t* conf)
{
    error_t err = kOk;

    err = buffer_pool_init(&messages, sizeof(message_t), conf->max_messages);
    if (err != kOk)
    {
        LOGE("failed to init buffer pool for messages, err%d\n", err);
        return kErr;
    }

    err = buffer_pool_init(&packets, sizeof(packet_t), conf->max_packets);
    if (err != kOk)
    {
        LOGE("failed to init buffer pool for packets, err%d\n", err);
        return kErr;
    }

    err = registered_users_init(&registered_users);
    if (err != kOk)
    {
        LOGE("failed to init registered users list , err%d\n", err);
        return kErr;
    }

    if (messages == NULL || packets == NULL)
    {
        LOGE("Failed to intialize buffer pool handles\n");
        return kErr;
    }

    for (int i = 0; i < conf->max_messages; i++)
    {
        if (messages->buffer_pool[i] == NULL)
        {
            LOGE("failed to init messages buffer pool\n");
            return kErr;
        }
    }

    for (int i = 0; i < conf->max_packets; i++)
    {
        if (packets->buffer_pool[i] == NULL)
        {
            LOGE("failed to init packets buffer pool\n");
            return kErr;
        }
    }

    for (int i = 0; i < packets->max_size; i++)
    {
        packet_t* pkt = (packet_t*)packets->buffer_pool[i];
        pkt->msg = (message_t*)malloc(sizeof(message_t));
    }

    return kOk;
}

error_t internal_term()
{
    error_t err = kOk;

    if (messages == NULL || packets == NULL || registered_users == NULL)
    {
        LOGE("system was not initalized properly");
        return kErr;
    }

    err = buffer_pool_destory(messages);
    if (err != kOk)
    {
        LOGE("Failed to destroy messages buffer pool properly, err=%d\n", err);
    }

    err = buffer_pool_destory(packets);
    if (err != kOk)
    {
        LOGE("Failed to destroy packets buffer pool properly, err=%d\n", err);
    }

    err = registered_users_destroy(registered_users);
    if (err != kOk)
    {
        LOGE("Failed to destroy registered users properly, err=%d\n", err);
    }

    messages = NULL;
    packets = NULL;
    registered_users = NULL;

    return err;
}

error_t get_new_message(message_t** msg)
{
    pthread_mutex_lock(&messages->mutex);
    if (messages->size == messages->max_size)
    {
        pthread_mutex_unlock(&messages->mutex);
        LOGE("No more messages available\n");
        return kErr;
    }

    for (int i = 0; i < messages->max_size; i++)
    {
        if (messages->occupied[i] == 0)
        {
            messages->size++;
            messages->occupied[i] = 1;
            *msg = (message_t*) messages->buffer_pool[i];
            LOGI("Got new message from buffer, p:%p, i:%d\n", *msg, i);
            pthread_mutex_unlock(&messages->mutex);
            return kOk;
        }
    }
    pthread_mutex_unlock(&messages->mutex);

    LOGE("No new messages left in buffer pool\n");
    return kErr;
}

error_t return_used_message(message_t* msg)
{
    if (msg == NULL)
    {
        LOGE("Given null message struct\n");
        return kErrParam;
    }

    pthread_mutex_lock(&messages->mutex);
    for (int i = 0; i < messages->max_size; i++)
    {
        if (messages->buffer_pool[i] == msg)
        {
            messages->size--;
            messages->occupied[i] = 0;
            LOGI("Returned message back to buffer, p:%p, i:%d\n", msg, i);
            pthread_mutex_unlock(&messages->mutex);
            return kOk;
        }
    }
    pthread_mutex_unlock(&messages->mutex);

    LOGE("Given msg was not found, %p\n", msg);
    return kErr;
}

error_t send_packet(uint8_t destination_id, message_t* msg)
{
    if (msg == NULL)
    {
        LOGE("given message is NULL\n");
        return kErrParam;
    }

    if(is_user_registered(destination_id) != kOk)
    {
        LOGE("the given destination=%d is not registered to the service\n",
            destination_id);
        return kErrParam;
    }

    pthread_mutex_lock(&packets->mutex);

    if (packets->size == packets->max_size)
    {
        pthread_mutex_unlock(&packets->mutex);
        LOGE("No more packets available\n");
        return kErr;
    }

    for (int i = 0; i < packets->max_size; i++)
    {
        if (packets->occupied[i] == 0)
        {
            packets->size++;
            packets->occupied[i] = 1;

            packet_t* packet = (packet_t*)packets->buffer_pool[i];
            packet->dst = destination_id;
            memcpy(packet->msg->data, msg->data, msg->len);
            packet->msg->len = msg->len;
            LOGD("Got new packet from buffer, p:%p, i:%d, msg=%p\n",
                packet, i, packet->msg);

            pthread_mutex_unlock(&packets->mutex);
            return kOk;
        }
    }

    pthread_mutex_unlock(&packets->mutex);

    LOGE("Given msg was not sent\n");
    return kErr;
}

error_t receive_packet(uint8_t receiver_id, message_t** msg)
{
    if(is_user_registered(receiver_id) != kOk)
    {
        LOGE("the given receiver_id=%d is not registered to the service\n",
            receiver_id);
        return kErrParam;
    }

    pthread_mutex_lock(&packets->mutex);

    for (int i = 0; i < packets->max_size; i++)
    {
        packet_t* pkt = (packet_t*)packets->buffer_pool[i];
        if (packets->occupied[i] == 1 && pkt->dst == receiver_id)
        {
            *msg = pkt->msg;
            packets->occupied[i] = 0;
            packets->size--;
            LOGI("Packet received, releasing..., p:%p, i:%d\n", pkt, i);
            pthread_mutex_unlock(&packets->mutex);
            return kOk;
        }
    }

    pthread_mutex_unlock(&packets->mutex);

    LOGW("No packet was received, rec_id=%d\n", receiver_id);
    return kErr;
}

error_t buffer_pool_init(buffer_pool_handle_t** handle, size_t data_size, uint8_t length)
{
    (*handle) = (buffer_pool_handle_t*)malloc(sizeof(buffer_pool_handle_t));
    if (*handle == NULL)
    {
        LOGE("failed to allocate memory for buffer_pool_handle=%p\n", *handle);
        return kErr;
    }

    (*handle)->buffer_pool = malloc(sizeof(void*) * length);
    if ((*handle)->buffer_pool == NULL)
    {
        LOGE("failed to allocate memory for buffer pool for buffer_pool_handle=%p\n", *handle);
        return kErr;
    }
    LOGI("intialized buffer pool, p=%p\n", (*handle)->buffer_pool);

    (*handle)->occupied = malloc(sizeof(uint8_t) * length);
    if ((*handle)->occupied == NULL)
    {
        LOGE("failed to allocate memory for occupied buffer_pool_handle=%p\n", *handle);
        return kErr;
    }

    (*handle)->size = 0;
    (*handle)->max_size = length;

    if (pthread_mutex_init(&(*handle)->mutex, NULL) != 0)
    {
        LOGE("failed to init mutex for buffer_pool_handle=%p\n", *handle);
        return kErr;
    }

    for (int i = 0; i < length; i++)
    {
        (*handle)->buffer_pool[i] = malloc(data_size);
        (*handle)->occupied[i] = 0;
        LOGI("created a new buffer, p=%p, i=%d\n", (*handle)->buffer_pool[i], i);
    }

    return kOk;
}

error_t buffer_pool_destory(buffer_pool_handle_t* handle)
{
    error_t ret_err = kOk;
    if (handle == NULL)
    {
        LOGE("The given handle is null\n");
        return kErr;
    }

    if (pthread_mutex_destroy(&handle->mutex) != 0)
    {
        LOGE("failed to destory buffer_pool_handle=%p mutex\n", handle);
        ret_err = kErr;
    };

    free(handle->buffer_pool);
    free(handle->occupied);
    free(handle);

    return ret_err;
}

error_t registered_users_init(linked_list_handle_t** users)
{
    (*users) = (linked_list_handle_t*)malloc(sizeof(linked_list_handle_t));
    if ((*users) == NULL)
    {
        LOGE("failed to allocate memory for users\n");
        return kErr;
    }

    (*users)->head_node = (node_t*)malloc(sizeof(node_t));
    if ((*users)->head_node == NULL)
    {
        LOGE("failed to allocate memory for head_node\n");
        return kErr;
    }

    (*users)->size = 0;
    (*users)->next_user_id = 1;
    (*users)->head_node->data = -1;
    (*users)->head_node->next = NULL;
    (*users)->head_node->prev = NULL;

    if (pthread_mutex_init(&(*users)->mutex, NULL) != 0)
    {
        LOGE("failed to init mutex for users\n");
        return kErr;
    }

    return kOk;
}

error_t registered_users_destroy(linked_list_handle_t* users)
{
    error_t ret_err = kOk;

    if (pthread_mutex_destroy(&users->mutex) != 0)
    {
        LOGE("failed to destroy uses mutex\n");
        ret_err = kErr;
    }

    node_t* head = users->head_node;
    while (head != NULL)
    {
        node_t* temp = head;
        head = head->next;
        free(temp);
    }

    free(users);

    return ret_err;
}

error_t register_new_user(uint8_t* new_id)
{
    pthread_mutex_lock(&registered_users->mutex);

    uint8_t gen_id = registered_users->next_user_id;
    registered_users->size++;
    registered_users->next_user_id++;

    struct node* new_node = (struct node*)malloc(sizeof(node_t));
    new_node->data = gen_id;
    new_node->next = NULL;

    node_t* temp_node = (registered_users->head_node);
    while (temp_node->next != NULL)
    {
        temp_node = temp_node->next;
    }

    temp_node->next = new_node;
    new_node->prev = temp_node;
    *new_id = gen_id;

    pthread_mutex_unlock(&registered_users->mutex);

    return kOk;
}

error_t deregister_user(uint8_t user_id)
{
    pthread_mutex_lock(&registered_users->mutex);

    node_t* del_node = registered_users->head_node;
    while (del_node->next != NULL && del_node->data != user_id)
    {
        del_node = del_node->next;
    }

    if (del_node->data != user_id)
    {
        LOGE("given user_id is not registered to the service, u_id=%d", user_id);
        pthread_mutex_unlock(&registered_users->mutex);
        return kErrParam;
    }

    if (del_node->next != NULL)
    {
        del_node->next->prev = del_node->prev;
    }
    if (del_node->prev != NULL)
    {
        del_node->prev->next = del_node->next;
    }

    pthread_mutex_unlock(&registered_users->mutex);

    return kOk;
}

error_t is_user_registered(uint8_t user_id)
{
    pthread_mutex_lock(&registered_users->mutex);

    node_t* temp_node = registered_users->head_node;
    while (temp_node != NULL)
    {
        if (temp_node->data == user_id)
        {
            pthread_mutex_unlock(&registered_users->mutex);
            return kOk;
        }
        temp_node = temp_node->next;
    }

    pthread_mutex_unlock(&registered_users->mutex);
    return kErr;
}

int check_internal_occupied(message_t* msg)
{
    if (msg == NULL)
    {
        LOGE("Given msg is NULL\n");
        return -1;
    }

    pthread_mutex_lock(&messages->mutex);

    for (int i = 0; i < messages->max_size; i++)
    {
        if (messages->buffer_pool[i] == msg)
        {
            pthread_mutex_unlock(&messages->mutex);
            return messages->occupied[i];
        }
    }

    pthread_mutex_unlock(&messages->mutex);

    return -1;
}
