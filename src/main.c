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

#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define DATA_TO_SEND "sending data"

void *thread1(void *args)
{

    char src[11] = "hello world";
    message_t* msg = new_message();
    memcpy(msg->data, src, strlen(src)+1);
    msg->len = strlen(src)+1;
    send(2, msg);

    return NULL;
}

void *thread2(void *args)
{
    message_t* msg;
    uint8_t rec_id = 2;

    while (recv(rec_id, &msg) != 0)
    {
        printf("Thread2 waiting...\n");
        sleep(1);
    }

    char src[12] = "";
    memcpy(src, msg->data, msg->len);
    printf("received message=%s, length=%d\n", src, msg->len);

    delete_message(msg);
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

    pthread_t thread1_id;
    pthread_t thread2_id;
    pthread_create(&thread1_id, NULL, thread1, NULL);
    pthread_create(&thread2_id, NULL, thread2, NULL);
    pthread_join(thread1_id, NULL);
    pthread_join(thread2_id, NULL);
    return 0;
}