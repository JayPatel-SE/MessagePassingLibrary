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

#include "utility/Types.h"

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
 * @return error_t Error code, kOk if successful
 */
error_t buffer_pool_init(
    buffer_pool_handle_t** handle, size_t data_size, uint8_t length);

/**
 * @brief Free all allocated resources of a given buffer pool
 *
 * @param[in] handle A pointer to a buffer pool
 * @return error_t Error code, kOk if successful
 */
error_t buffer_pool_destory(buffer_pool_handle_t* handle);

/**
 * @brief Intialize the linked list for all registered users
 *
 * @param[in] users reference to the linked list handle
 * @return error_t Error code, kOk if successful
 */
error_t registered_users_init(linked_list_handle_t** users);

/**
 * @brief Free all allocated memory resources of a given linked list
 *
 * @param[in] users pointer to the linked list
 * @return error_t Error code, kOk if successful
 */
error_t registered_users_destroy(linked_list_handle_t* users);

/**
 * @brief register a new user to the service
 *
 * @param[out] new_id the newly generated id
 * @return error_t Error code, kOk if successful
 */
error_t register_new_user(uint8_t* new_id);

/**
 * @brief deregister a given user from teh service
 *
 * @param[in] user_id the user id to deregister
 * @return error_t Error code, kOk if successful
 */
error_t deregister_user(uint8_t user_id);

/**
 * @brief Determine if a user is registered to the system
 *
 * @param[in] user_id the user id to check
 * @return int 0 if not registered, 1 if registered
 */
int is_user_registered(uint8_t user_id);

/**
 * @brief This is ONLY FOR TESTING. Used to check if the given message_t* is
 *        occupied or not.
 *
 * @param[in] msg pointer to the message
 * @return int 0 is not occupied, 1 if occupied, -1 error
 */
int check_internal_occupied(message_t* msg);

#endif
