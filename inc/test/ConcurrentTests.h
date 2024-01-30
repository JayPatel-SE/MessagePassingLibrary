/**
 * @file ConcurrentTests.h
 * @author Jay Patel
 * @brief Concurreny tests for the system
 * @version 0.1
 * @date 2024-01-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef CONCURRENT_TESTS_H
#define CONCURRENT_TESTS_H

#include "test/TestHelpers.h"

#include <unistd.h>

typedef struct {
    uint8_t rec_id;
    int* thread_ids;
    int num_threads;
    int expect_packets;
} test_thread_params_t;

void *send_thread(void* args)
{
    test_thread_params_t* params = (test_thread_params_t*) args;
    int num_chars = 12;
    message_t* msgs[params->num_threads];

    for (int i = 0; i < params->num_threads; i++)
    {
        int num_digits = ((i+1) == 0) ? 1 : log10((i+1))+1;
        int total_size = num_chars + num_digits + 1;
        char src[total_size];

        // get a message from system buffer
        msgs[i] = new_message();
        snprintf(src, total_size, "hello world %d", (i+1));

        // fill in message
        memcpy(msgs[i]->data, src, total_size+1);
        msgs[i]->len = total_size+1;
        printf("Added str=%s, len=%d, to msg\n", src, total_size);

        // send message
        send((i+1), msgs[i]);

        // delete message
        delete_message(msgs[i]);
    }

    return NULL;
}

static void setup_sender_thread(int num_rec_threads, test_thread_params_t* params)
{
    params->num_threads = num_rec_threads;
    params->thread_ids = (int*)malloc(sizeof(int) * num_rec_threads);
    for (int i = 0; i < num_rec_threads; i++)
    {
        params->thread_ids[i] = register_user();
    }
}

void *send_test_1(void* args)
{
    test_thread_params_t* params = (test_thread_params_t*) args;
    int num_chars = 12;
    message_t* msgs[params->num_threads];

    for (int i = 0; i < params->num_threads; i++)
    {
        int rec_id = params->thread_ids[i];
        int num_digits = ((rec_id) == 0) ? 1 : log10((rec_id))+1;
        int total_size = num_chars + num_digits + 1;
        char src[total_size];

        // get a message from system buffer
        msgs[i] = new_message();
        assert_non_null(msgs[i]);
        snprintf(src, total_size, "hello world %d", (i+1));

        // fill in message
        memcpy(msgs[i]->data, src, total_size+1);
        msgs[i]->len = total_size+1;

        // send message
        assert_int_equal(send((rec_id), msgs[i]), 0);

        // delete message
        delete_message(msgs[i]);
    }

    return NULL;
}

void *rec_test_1(void* args)
{
    test_thread_params_t* params = (test_thread_params_t*) args;
    message_t msg;
    uint8_t rec_id = params->rec_id;

    // receive message
    while (recv(rec_id, &msg) != 0)
    {
        // printf("Thread%d waiting...\n", rec_id);
        sleep(1);
    }

    int num_chars = 12;
    int num_digits = (rec_id == 0) ? 1 : log10(rec_id)+1;
    int total_size = num_chars + num_digits + 1;
    char expected_string[total_size];
    snprintf(expected_string, total_size, "hello world %d", rec_id);

    char src[msg.len];
    memcpy(src, msg.data, msg.len);
    // printf("received message=%s, length=%d\n", src, msg->len);
    assert_string_equal(src, expected_string);
    assert_int_equal(msg.len, total_size + 1);

    return NULL;
}

void *receive_twice(void* args)
{
    test_thread_params_t* params = (test_thread_params_t*) args;
    message_t msg;
    uint8_t rec_id = params->rec_id;

    // receive message
    while (recv(rec_id, &msg) != 0)
    {
        // printf("Thread%d waiting...\n", rec_id);
        sleep(1);
    }

    int num_chars = 12;
    int num_digits = (rec_id == 0) ? 1 : log10(rec_id)+1;
    int total_size = num_chars + num_digits + 1;
    char expected_string[total_size];
    snprintf(expected_string, total_size, "hello world %d", rec_id);

    char src[msg.len];
    memcpy(src, msg.data, msg.len);
    // printf("received message=%s, length=%d\n", src, msg->len);
    assert_string_equal(src, expected_string);
    assert_int_equal(msg.len, total_size + 1);

    // try and receive packet again
    for (int i = 0; i < 5; i++)
    {
        int err = recv(rec_id, &msg);
        assert_int_not_equal(err, 0);
        sleep(1);
    }

    return NULL;
}

/* Test sending one msg from one thread to another */
static void one_send_one_receive(void **state)
{
    int num_threads = 1;
    setup_service(10, 10);

    // setup the sender thread
    pthread_t send_thread_id;
    test_thread_params_t send_params;
    setup_sender_thread(num_threads, &send_params);
    pthread_create(&send_thread_id, NULL, send_test_1, &send_params);

    // setup the receiver thread
    pthread_t rec_threads[num_threads];
    test_thread_params_t rec_params[num_threads];
    for(int i = 0; i < num_threads; i++)
    {
        rec_params[i].rec_id = send_params.thread_ids[i];
        pthread_create(&rec_threads[i], NULL, rec_test_1, &rec_params[i]);
    }

    pthread_join(send_thread_id, NULL);
    for(int i = 0; i < num_threads; i++)
    {
        pthread_join(rec_threads[i], NULL);
    }

    teardown_service();
}

/* Test sending one msg to multiple thread from one sender */
static void one_send_multiple_receive(void **state)
{
    int num_threads = 10;
    setup_service(10, 10);

    // setup the sender thread
    pthread_t send_thread_id;
    test_thread_params_t send_params;
    setup_sender_thread(num_threads, &send_params);
    pthread_create(&send_thread_id, NULL, send_test_1, &send_params);

    // setup the receiver thread
    pthread_t rec_threads[num_threads];
    test_thread_params_t rec_params[num_threads];
    for(int i = 0; i < num_threads; i++)
    {
        rec_params[i].rec_id = send_params.thread_ids[i];
        pthread_create(&rec_threads[i], NULL, rec_test_1, &rec_params[i]);
    }

    pthread_join(send_thread_id, NULL);
    for(int i = 0; i < num_threads; i++)
    {
        pthread_join(rec_threads[i], NULL);
    }

    teardown_service();
}

/* Test sending one msg to one thread, and try to receive two messages
 * The first receive should pass, the second receive should fail.
 */
static void send_once_receive_twice(void **state)
{
    setup_service(10, 10);

    pthread_t send_thread_id;
    pthread_t recv_thread_id;
    test_thread_params_t params;
    params.num_threads = 1;
    params.thread_ids = (int*)malloc(sizeof(int) * params.num_threads);
    params.thread_ids[0] = register_user();
    pthread_create(&send_thread_id, NULL, send_test_1, &params);
    params.rec_id =  1;
    pthread_create(&recv_thread_id, NULL, receive_twice, &params);

    pthread_join(send_thread_id, NULL);
    pthread_join(recv_thread_id, NULL);
}

static const struct CMUnitTest basic_tests[] =
{
    cmocka_unit_test(one_send_one_receive),
    cmocka_unit_test(one_send_multiple_receive),
    cmocka_unit_test(send_once_receive_twice),
};

#endif
