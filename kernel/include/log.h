/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>

#include "printf.h"

#define LOG_VERSION "0.1.0"
#define LOG_USE_COLOR 1

typedef struct {
  va_list ap;
  const char *fmt;
  const char *file;
  // struct tm *time;
  // void *udata; // stderr ...
  int line;
  int level;
} log_Event;

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#define panic(...)                                       \
  do {                                                   \
    log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__); \
    while (1)                                            \
      ;                                                  \
  } while (0)

#define assert(x)                      \
  do {                                 \
    if ((int)(x) == 1) {               \
      ;                                \
    } else {                           \
      log_error("assert failed: " #x); \
      while (1)                        \
        ;                              \
    }                                  \
  } while (0)

const char *log_level_string(int level);
void log_set_level(int level);
void log_set_quiet(bool enable);
void log_init();
void log_log(int level, const char *file, int line, const char *fmt, ...);

#endif
