/**
 * @file main.c
 * @author Jay Patel
 * @brief Main file for the program
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "test/APITests.h"
#include "test/ConcurrentTests.h"
#include "MessageService.h"
#include "utility/log.h"

#include <pthread.h>

int main(void)
{
    // 1. Setup Message Service
    int max_messages = 10;
    int max_packets = 10;
    system_conf_t conf;
    conf.max_messages = max_messages;
    conf.max_packets = max_packets;

    // 2. Initialize the message system
    error_t err = message_system_init(&conf);
    if (err != kOk)
    {
        printf("Failed to init message system, err=%d\n", err);
        return -1;
    }

    // 3. Register user
    int rec_id = register_user();

    // 4. Get new message
    message_t* msg = new_message();

    // 5. Fill in Data
    int num_chars = 12;
    int num_digits = (rec_id == 0) ? 1 : log10(rec_id)+1;
    int total_size = num_chars + num_digits + 1;
    char src[total_size];

    snprintf(src, total_size, "hello world %d", rec_id);
    memcpy(msg->data, src, total_size);
    msg->len = total_size;

    // 6. Send the message
    send(rec_id, msg);

    // 7. Delete message
    delete_message(msg);

    // 8. Receive message
    message_t rec_msg;
    recv(rec_id, &rec_msg);
    char rec_src[rec_msg.len];
    memcpy(rec_src, rec_msg.data, rec_msg.len);
    printf("received message=%s, length=%d\n", rec_src, rec_msg.len);

    // 9. Teardown system
    message_system_term();

    // run tests
    cmocka_run_group_tests_name("api_tests", api_tests, NULL, NULL);
    cmocka_run_group_tests_name("basic_tests", basic_tests, NULL, NULL);

    return 0;
}
