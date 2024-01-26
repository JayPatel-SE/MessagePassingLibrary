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

#include <stdio.h>
#include <stdlib.h>


/** Error Defintions */
typedef enum {
    kOk = 0,//!< NO ERROR
    kErr  = 1,//!< GENERIC ERROR
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
    uint8_t number_of_messages;
    uint8_t size_of_messages;
    uint16_t max_messages;
    uint8_t max_threads;
} system_conf_t;

/** A counter to assign an unique identifier to new threads*/
typedef struct
{
    int* registered_users;
    int next_unique_id;
    pthread_mutex_t mutex;
} new_user_id_t;
