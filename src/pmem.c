#include "pmem.h"


physical_memory_t* physical_memory_create(uint32_t total_memory_bytes) {
  return ;
}

int32_t physical_frame_alloc(physical_memory_t* pmem) {
  return 0;
}

void physical_frame_free(physical_memory_t* pmem, uint32_t frame_number) {
}

void physical_memory_destroy(physical_memory_t* pmem) {
}
