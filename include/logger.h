#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <stdio.h>
#define LOG_ERROR(format, ...) \
  fprintf(stderr, "[ERROR] (%s:%d): " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)


#endif