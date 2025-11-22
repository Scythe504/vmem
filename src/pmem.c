#include "pmem.h"

#include <math.h>
#include "page.h"
#include "tlb.h"

#include "logger.h"

#define PAGE_SIZE_BYTES (4ULL << 10)  // 4 KiB = 4096 bytes (size of a page)

physical_memory_t* physical_memory_create(uint32_t total_memory_bytes) {
  physical_memory_t* pmem = calloc(1, sizeof(physical_memory_t));
  if (pmem == NULL) {
    LOG_ERROR("Failed to allocate physical_memory_t structure.");
    return NULL;
  }

  uint32_t total_frames = total_memory_bytes / PAGE_SIZE_BYTES;
  size_t bitmap_size_bytes = total_frames / 8;

  uint8_t* bitmap_ptr = (uint8_t*)calloc(bitmap_size_bytes, sizeof(uint8_t));
  if (bitmap_ptr == NULL) {
    LOG_ERROR("Failed to allocate %zu bytes for PMM bitmap.", bitmap_size_bytes);
    free(pmem);
    return NULL;
  }

  pmem->bitmap = bitmap_ptr;
  pmem->total_frames = total_frames;
  pmem->used_frames = 0;

  return pmem;
}

// Allocate a free physical frame, return frame number (or -1 if none free)
int32_t physical_frame_alloc(physical_memory_t* pmem) {
  uint32_t total_bytes = pmem->total_frames / 8;

  for (unsigned byte_idx = 0; byte_idx < total_bytes; byte_idx++) {
    uint8_t byte = pmem->bitmap[byte_idx];
    // if all bits are 1 (i.e no free space)
    if (byte == 0xFF) {
      continue;
    }

    for (unsigned bit_idx = 0; bit_idx < 8; bit_idx++) {
      uint8_t mask = (1 << bit_idx);

      // true if bit is 0
      if (!(byte & mask)) {
        uint32_t frame_number = (byte_idx * 8) + bit_idx;

        pmem->bitmap[byte_idx] |= mask;

        pmem->used_frames++;

        return frame_number;
      }
    }
  }

  // If loop finishes without finding a free frame
  LOG_ERROR("Physical Memory Exhausted! No free frames available.");
  return -1;
}

void physical_frame_free(physical_memory_t* pmem, uint32_t frame_number) {
  uint8_t byte_idx = frame_number / 8;
  uint8_t mask = frame_number % 8;
  uint8_t bit = pmem->bitmap[byte_idx] & (1 << mask);

  if (bit == 0) {
    LOG_ERROR("Unsetting Already unset bit:%u, byte_idx:%u bit_idx:%u\n", bit, byte_idx, mask);
    return;
  }

  pmem->used_frames--;
  pmem->bitmap[byte_idx] &= ~(1 << mask);
}

void physical_memory_destroy(physical_memory_t* pmem) {
  if (pmem == NULL) {
    LOG_ERROR("Physical Memory is already NULL: %p", pmem);
    return;
  }

  free(pmem);
}