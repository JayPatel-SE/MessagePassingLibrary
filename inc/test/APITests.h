/**
 * @file APITests.c
 * @author Jay Patel
 * @brief Basic tests for the message system
 * @version 0.1
 * @date 2024-01-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "test/TestHelpers.h"

#include <stdint.h>
#include <stdlib.h>

#ifndef API_TESTS_H
#define API_TESTS_H

/* Testing the system_init() function call of the API */
static void system_init_test(void** state)
{
    {
        // system init will null conf, should return err
        system_conf_t* conf_null = NULL;
        error_t err = message_system_init(conf_null);
        assert_int_not_equal(err, 0);
        err = message_system_term();
        assert_int_not_equal(err, 0);
    }

    {
        // system init with initialized conf, should pass
        system_conf_t conf;
        conf.max_messages = 10;
        conf.max_packets = 10;
        error_t err = message_system_init(&conf);
        assert_int_equal(err, 0);
        err = message_system_term();
        assert_int_equal(err, 0);
    }

}

/* Testing the new_message() function call of the API */
static void new_message_tests(void **state)
{
    {
        // Get new message when there are messages available
        setup_service(10, 10);
        message_t* msg = NULL;
        msg = new_message();
        assert_non_null(msg);
        teardown_service();
    }

    {
        // Get max number of new messages
        int number_of_messages = 100;
        setup_service(number_of_messages, number_of_messages);
        message_t* msg[number_of_messages];

        for (int i = 0; i < number_of_messages; i++)
        {
            msg[i] = new_message();
            assert_non_null(msg[i]);
        }
        teardown_service();
    }

    {
        // Get more than max number of new messages
        int number_of_messages = 100;
        int extra_messages = 10;
        setup_service(number_of_messages, number_of_messages);
        message_t* msg[number_of_messages + extra_messages];

        for (int i = 0; i < number_of_messages; i++)
        {
            msg[i] = new_message();
            assert_non_null(msg[i]);
        }

        for (int i = number_of_messages;
            i < number_of_messages + extra_messages; i++)
        {
            msg[i] = new_message();
            assert_null(msg[i]);
        }

        teardown_service();
    }
}

/* Testing the delete_message() function call of the API */
static void delete_message_tests(void **state)
{
    {
        // Try to delete a NULL msg, program should not crash
        setup_service(10, 10);
        delete_message(NULL);
        teardown_service();
    }

    {
        // Normal delete case
        int number_of_messages = 100;
        setup_service(number_of_messages, number_of_messages);
        message_t* msg[number_of_messages];

        for (int i = 0; i < number_of_messages; i++)
        {
            msg[i] = new_message();
            assert_non_null(msg[i]);
        }

        for (int i = 0; i < number_of_messages; i++)
        {
            delete_message(msg[i]);
            assert_non_null(msg[i]);
            assert_int_equal(check_occupied(msg[i]), 0);
        }

        teardown_service();
    }

    {
        // Delete a message that isn't a part of the system, program should
        // not crash
        message_t* msg = (message_t*)malloc(sizeof(message_t));
        setup_service(10, 10);
        delete_message(msg);
        teardown_service();
    }
}

/* Testing the send() function call of the API */
static void send_tests(void **state)
{
    {
        // Test deafult case, with all correct args and scenario
        int num_messages = 10;
        int num_users = 1;
        setup_service(num_messages, num_messages);
        message_t* msg[num_users];
        int registered_users[num_users];

        for (int i = 0; i < num_users; i++)
        {
            registered_users[i] = register_user();
        }

        for (int i = 0; i < num_users; i++)
        {
            msg[i] = new_message();
            assert_non_null(msg[i]);
        }

        for (int i = 0; i < num_users; i++)
        {
            int err = send(registered_users[i], msg[i]);
            assert_int_equal(err, 0);
        }

        for (int i = 0; i < num_users; i++)
        {
            delete_message(msg[i]);
            assert_non_null(msg[i]);
            assert_int_equal(check_occupied(msg[i]), 0);
            remove_user(registered_users[i]);
        }

        teardown_service();
    }

    {
        // Send with null message
        int num_messages = 10;
        int num_users = 1;
        setup_service(num_messages, num_messages);
        message_t* msg[num_users];
        int registered_users[num_users];

        for (int i = 0; i < num_users; i++)
        {
            registered_users[i] = register_user();
        }

        for (int i = 0; i < num_users; i++)
        {
            msg[i] = new_message();
            assert_non_null(msg[i]);
        }

        for (int i = 0; i < num_users; i++)
        {
            int err = send(registered_users[i], NULL);
            assert_int_not_equal(err, 0);
        }

        for (int i = 0; i < num_users; i++)
        {
            delete_message(msg[i]);
            assert_non_null(msg[i]);
            assert_int_equal(check_occupied(msg[i]), 0);
            remove_user(registered_users[i]);
        }

        teardown_service();
    }

    {
        // Send to a id that is not registered
        int num_messages = 10;
        int num_users = 1;
        setup_service(num_messages, num_messages);
        message_t* msg[num_users];
        int registered_users[num_users];

        for (int i = 0; i < num_users; i++)
        {
            registered_users[i] = register_user();
        }

        for (int i = 0; i < num_users; i++)
        {
            msg[i] = new_message();
            assert_non_null(msg[i]);
        }

        for (int i = 0; i < num_users; i++)
        {
            int err = send(-1, NULL);
            assert_int_not_equal(err, 0);
        }

        for (int i = 0; i < num_users; i++)
        {
            delete_message(msg[i]);
            assert_non_null(msg[i]);
            assert_int_equal(check_occupied(msg[i]), 0);
            remove_user(registered_users[i]);
        }

        teardown_service();
    }
}

/* Testing the recv() function all of the API */
static void recv_tests(void **state)
{
    {
        // default correct use case of recv
        int num_messages = 10;
        int num_users = 1;
        message_t* msg[num_users];
        int registered_users[num_users];

        setup_and_send_msgs(num_messages, num_users, registered_users, msg);
        recevie_msgs(num_users, registered_users, msg, 0);
        teardown_msgs(num_users, registered_users, msg);

        teardown_service();
    }

    {
        // recevie on an id that is not registered
        int num_messages = 10;
        int num_users = 1;
        message_t* msg[num_users];
        int registered_users[num_users];

        setup_and_send_msgs(num_messages, num_users, registered_users, msg);
        registered_users[0] = num_users + 12;
        recevie_msgs(num_users, registered_users, msg, 1);
        teardown_msgs(num_users, registered_users, msg);

        teardown_service();
    }
}

static const struct CMUnitTest api_tests[] =
{
    cmocka_unit_test(system_init_test),
    cmocka_unit_test(new_message_tests),
    cmocka_unit_test(delete_message_tests),
    cmocka_unit_test(send_tests),
    cmocka_unit_test(recv_tests),
};

#endif
