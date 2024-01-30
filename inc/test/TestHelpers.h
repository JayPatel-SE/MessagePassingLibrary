/**
 * @file TestHelpers.h
 * @author Jay Patel
 * @brief All the helper fucntions for the tests
 * @version 0.1
 * @date 2024-01-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "MessageService.h"

#include <math.h>
#include <string.h>

// libraries for testing
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

static void setup_service(uint16_t max_messages, uint16_t max_packets)
{
    // Setup the message system configuration
    system_conf_t conf;
    conf.max_messages = max_messages;
    conf.max_packets = max_packets;

    // Initialize the message system
    error_t err = message_system_init(&conf);
    if (err != kOk)
    {
        printf("Failed to init message system, err=%d\n", err);
        return;
    }
}

static void teardown_service()
{
    // Teardown the Message System to free all memory
    message_system_term();
}

static void setup_and_send_msgs(
    int num_msgs, int num_usrs, int registered_users[], message_t* msg[])
{
        // default correct use case of recv
        int num_messages = num_msgs;
        int num_users = num_usrs;
        setup_service(num_messages, num_messages);

        for (int i = 0; i < num_users; i++)
        {
            registered_users[i] = register_user();
        }

        for (int i = 0; i < num_users; i++)
        {
            msg[i] = new_message();
            assert_non_null(msg[i]);
        }

        int num_chars = 12;
        for (int i = 0; i < num_users; i++)
        {
            int rec_id = registered_users[i];
            int num_digits = (rec_id == 0) ? 1 : log10(rec_id)+1;
            int total_size = num_chars + num_digits + 1;
            char src[total_size];
            snprintf(src, total_size, "hello world %d", rec_id);

            // fill in message
            memcpy(msg[i]->data, src, total_size+1);
            msg[i]->len = total_size+1;

            // send message
            int err = send(rec_id, msg[i]);
            assert_int_equal(err, 0);
        }
}

void teardown_msgs(int num_users, int registered_users[], message_t* msg[])
{
    for (int i = 0; i < num_users; i++)
    {
        delete_message(msg[i]);
        assert_non_null(msg[i]);
        assert_int_equal(check_occupied(msg[i]), 0);
        remove_user(registered_users[i]);
    }
}

void recevie_msgs(
    int num_users, int registered_users[], message_t* msg[], int expect_fail)
{
    int num_chars = 12;
    for (int i = 0; i < num_users; i++)
    {
        message_t msg;
        int rec_id = registered_users[i];
        int num_digits = (rec_id == 0) ? 1 : log10(rec_id)+1;
        int total_size = num_chars + num_digits + 1;
        char expected_string[total_size];
        snprintf(expected_string, total_size, "hello world %d", rec_id);

        int err = recv(rec_id, &msg);
        if (expect_fail == 1)
        {
            assert_int_not_equal(err, 0);
        }
        else
        {
            assert_int_equal(err, 0);
            char src[msg.len];
            memcpy(src, msg.data, msg.len);
            assert_string_equal(src, expected_string);
            assert_int_equal(msg.len, total_size + 1);
        }
    }
}


#endif