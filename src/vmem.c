#include <stdio.h>

#define LOG_ERROR(format, ...) \
  fprintf(stderr, "[ERROR] (%s:%d): " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

int main() {
  printf("Hello, World!");
  return 0;
}