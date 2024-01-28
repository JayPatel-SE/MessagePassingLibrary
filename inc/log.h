/**
 * @file log.h
 * @author Jay Patel
 * @brief Simple Log Service
 * @version 0.1
 * @date 2024-01-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <stdio.h>

#define ENABLE_ERROR_LOGS 1     // 0 to disbale ERROR logs, 1 to enable
#define ENABLE_INFO_LOGS 0      // 0 to disable INFO logs, 1 to enable

#ifndef LOG_H
#define LOG_H

#if ENABLE_ERROR_LOGS == 1
#define LOGE(...)  printf("ERROR: " __VA_ARGS__);
#else
#define LOGE(...) // do nothing
#endif

#if ENABLE_INFO_LOGS == 1
#define LOGI(...)  printf("INFO: " __VA_ARGS__);
#else
#define LOGI(...) // do nothing
#endif

#endif
