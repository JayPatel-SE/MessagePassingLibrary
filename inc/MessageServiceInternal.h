/**
 * @file MessageServiceInternal.h
 * @author Jay Patel
 * @brief The internal of the message service hidden from the user
 * @version 0.1
 * @date 2024-01-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "CircularQueue.h"
#include "Types.h"

#include <stdlib.h>
#include <stdint.h>

// Keep track of messages being used, 0 not used, 1 in used
static uint8_t* message_use;
// A queue of all of the messages sent ready to be received
static circular_queue_handle* packets;
// A queue of available messages
static circular_queue_handle* messages;
// Allocation a region of memory for messages
static void * messages_pool;

// A counter to assign unique identifier to new threads
static new_user_id_t* id_gen;

error_t internal_init(system_conf_t* conf);

error_t message_pool_init(void);

error_t send_packet(uint8_t destination_id, message_t* msg);

error_t receive_packet(uint8_t receiver_id, message_t* msg);

error_t get_new_message(message_t* msg);

error_t return_used_message(message_t* msg);

int get_new_id(void);