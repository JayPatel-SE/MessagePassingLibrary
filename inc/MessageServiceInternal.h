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

#include "Types.h"

#include <stdint.h>

#ifndef MESSAGE_SERVICE_INTERNAL_H
#define MESSAGE_SERVICE_INTERNAL_H

error_t internal_init(system_conf_t* conf);

error_t send_packet(uint8_t destination_id, message_t* msg);

error_t receive_packet(uint8_t receiver_id, message_t** msg);

error_t get_new_message(message_t** msg);

error_t return_used_message(message_t* msg);

void buffer_pool_init(
    buffer_pool_handle_t** handle, size_t data_size, uint8_t length);

void buffer_pool_destory(buffer_pool_handle_t* handle);

#endif
