/**
 * @file MessageService.h
 * @author Jay Patel
 * @brief API for message passing library
 * @version 0.1
 * @date 2024-01-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "Types.h"

#include <stdio.h>
#include <stdint.h>

#ifndef MESSAGE_SERVICE_H
#define MESSAGE_SERVICE_H

/**
 * @brief Initialize the Message Service System
 *
 * @param[in] conf Configuration for the system
 * @return error_t
 */
error_t message_system_init(system_conf_t* conf);

/**
 * @brief Get an available message struct from the library
 *
 * @return message_t* return a message struct else NULL
 */
message_t* new_message(void);

/**
 * @brief Return a message struct back to the message library
 *
 * @param[in] msg The message being returned to the library
 */
void delete_message(message_t* msg);

/**
 * @brief Send a message to a given destination.
 *
 * @param[in] destination_id The ID of the thread that will receive the message
 * @param[in] msg The message to be delivered
 * @return int 0 if success, non-0 if error
 */
int send(uint8_t destination_id, message_t* msg);

/**
 * @brief Receive any pending incoming messages
 *
 * @param[in] receiver_id The ID of the thread that wishes to recevie the message
 * @param[out] msg A reference to the pointer for the message being received
 * @return int 0 if success, non-0 if error
 */
int recv(uint8_t receiver_id, message_t** msg);

#endif
