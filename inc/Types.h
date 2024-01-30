/**
 * @file Types.h
 * @author Jay Patel
 * @brief This file contains the enums used in this project
 * @version 0.1
 * @date 2024-01-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <pthread.h>
#include <stdint.h>

#ifndef TYPES_H
#define TYPES_H

/** Error Defintions */
typedef enum {
    kOk = 0,
    kErr  = 1,
    kErrParam = 2,
} error_t;

/** Structure of the Message */
typedef struct
{
    uint8_t len;
    uint8_t data[255];
} message_t;

/** Packet for message transfer */
typedef struct
{
    uint8_t dst;
    message_t* msg;
} packet_t;

/** Configuration for the message system */
typedef struct
{
    uint16_t max_messages;
    uint16_t max_packets;
} system_conf_t;

/** Structure to hold buffer pool */
typedef struct
{
    void** buffer_pool;
    uint8_t* occupied;
    uint8_t size;
    uint8_t max_size;
    pthread_mutex_t mutex;
} buffer_pool_handle_t;

/* Node of a linked list */
typedef struct node
{
    int data;
    struct node* next;
    struct node* prev;
} node_t;

/* handle for a doubly linked list */
typedef struct
{
    node_t* head_node;
    int size;
    uint8_t next_user_id;
    pthread_mutex_t mutex;
} linked_list_handle_t;

typedef struct
{
    uint8_t next_user_id;
} user_id_generator_t;

#endif
