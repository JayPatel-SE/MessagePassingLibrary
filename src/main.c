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
#include <unistd.h>

void *thread1(void *args)
{
    for (int i = 0; i < 3; i++)
    {
        printf("test %d\n", i);
        sleep(1);
    }

    return NULL;
}

int main(void)
{
    pthread_t thread_id;
    printf("before executing the thread\n");
    pthread_create(&thread_id, NULL, thread1, NULL);
    pthread_join(thread_id, NULL);
    printf("After thread\n");
    return 0;
}