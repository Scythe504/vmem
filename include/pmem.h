#ifndef __PMEM_H_
#define __PMEM_H_
#include <stdlib.h>
#include <stdint.h>

#define PAGE_SIZE_BYTES (4ULL << 10)  // 4 KiB = 4096 bytes (size of a page)

typedef struct {
  uint8_t* bitmap;  // Each bit = 1 frame (0=free, 1=used)
  uint32_t total_frames;
  uint32_t used_frames;
} physical_memory_t;

// Initialize physical memory tracker
physical_memory_t* physical_memory_create(uint32_t total_memory_bytes);

// Allocate a free physical frame, return frame number (or -1 if none free)
int32_t physical_frame_alloc(physical_memory_t* pmem);

// Free a physical frame
void physical_frame_free(physical_memory_t* pmem, uint32_t frame_number);

// Destroy physical memory tracker
void physical_memory_destroy(physical_memory_t* pmem);

#endif
