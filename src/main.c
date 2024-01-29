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

#include "MessageService.h"
#include "log.h"

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DATA_TO_SEND "sending data"

typedef struct {
    uint8_t rec_id;
    int* thread_ids;
    int num_threads;
} thread_params_t;

void *send_thread(void* args)
{
    thread_params_t* params = (thread_params_t*) args;
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

void *receive_thread(void* args)
{
    thread_params_t* params = (thread_params_t*) args;
    message_t* msg;
    uint8_t rec_id = params->rec_id;

    while (recv(rec_id, &msg) != 0)
    {
        printf("Thread%d waiting...\n", rec_id);
        sleep(1);
    }

    char src[msg->len];
    memcpy(src, msg->data, msg->len);
    printf("received message=%s, length=%d\n", src, msg->len);

    return NULL;
}

int main(void)
{
    // Setup the message system configuration
    system_conf_t conf;
    conf.max_messages = 50;
    conf.max_packets = 50;

    // Initialize the message system
    error_t err = message_system_init(&conf);
    if (err != kOk)
    {
        printf("Failed to init message system, err=%d\n", err);
        return -1;
    }

    // setup the sender thread id
    int num_threads = 20;
    thread_params_t params_send;
    params_send.num_threads = num_threads;
    params_send.thread_ids = malloc(sizeof(int) * num_threads);
    for (int i = 0; i < num_threads; i++)
    {
        params_send.thread_ids[i] = (i+1);
    }

    pthread_t send_thread_id;
    pthread_create(&send_thread_id, NULL, send_thread, &params_send);

    // setup the receiver threads
    pthread_t rec_threads[num_threads];
    thread_params_t rec_params[num_threads];
    for(int i = 0; i < num_threads; i++)
    {
        params_send.rec_id = (i+1);
        rec_params[i].rec_id = (i+1);
        pthread_create(&rec_threads[i], NULL, receive_thread, &rec_params[i]);
    }

    pthread_join(send_thread_id, NULL);
    for(int i = 0; i < num_threads; i++)
    {
        pthread_join(rec_threads[i], NULL);
    }

    // Teardown the Message System to free all memory
    message_system_term();

    return 0;
}