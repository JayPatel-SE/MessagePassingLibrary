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

/**
 * @brief Initialize the internals of the message service system
 *
 * @param[in] conf The configuration of the system
 * @return error_t error code
 */
error_t internal_init(system_conf_t* conf);

/**
 * @brief Terminate the internals of the message service system
 *
 * @return error_t error code
 */
error_t internal_term();

/**
 * @brief Send a packet from one thread to another
 *
 * @param[in] destination_id The destination of the packet
 * @param[in] msg The message to contain in the packet
 * @return error_t error code
 */
error_t send_packet(uint8_t destination_id, message_t* msg);

/**
 * @brief Receive a packet that was sent for given receiver id
 *
 * @param[in] receiver_id The receiver of the packet
 * @param[out] msg pointer to message that will be received
 * @return error_t error code
 */
error_t receive_packet(uint8_t receiver_id, message_t** msg);

/**
 * @brief Get a new message from the internal system
 *
 * @param[out] msg pointer to the new message
 * @return error_t error code
 */
error_t get_new_message(message_t** msg);

/**
 * @brief Return a used message to the system's buffer pool
 *
 * @param[in] msg pointer to the message being returned
 * @return error_t error code
 */
error_t return_used_message(message_t* msg);

/**
 * @brief Initialize a buffer pool with a given buffer size
 *
 * @param[out] handle a reference to the pointer to the buffer pool
 * @param[in] data_size the size of each buffer
 * @param[in] length the number of buffers in the pool
 */
void buffer_pool_init(
    buffer_pool_handle_t** handle, size_t data_size, uint8_t length);

/**
 * @brief Free all allocated resources of a given buffer pool
 *
 * @param[in] handle A pointer to a buffer pool
 */
void buffer_pool_destory(buffer_pool_handle_t* handle);

#endif
