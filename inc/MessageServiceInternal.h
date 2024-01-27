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

#include <stdlib.h>
#include <stdint.h>

error_t internal_init(system_conf_t* conf);

error_t send_packet(uint8_t destination_id, message_t* msg);

error_t receive_packet(uint8_t receiver_id, message_t* msg);

error_t get_new_message(message_t* msg);

error_t return_used_message(message_t* msg);

int get_new_id(void);