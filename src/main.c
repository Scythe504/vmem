#include <stdio.h>

#include "page.h"
#include "pmem.h"
#include "stats.h"
#include "translate.h"

int main() {
  for (int i = 0; i < 3; i++) {
    page_directory_t* dir = page_directory_create();
    if (dir == NULL) {
      LOG_ERROR("DIR CREATION FAILED");
      return 1;
    }

    physical_memory_t* pmem = physical_memory_create(1024 * 1024 * 1024);
    if (pmem == NULL) {
      LOG_ERROR("PMEM CREATION FAILED");
      return 1;
    }
    tlb_t* tlb = tlb_create();
    if (tlb == NULL) {
      LOG_ERROR("TLB CREATION FAILED");
      return 1;
    }

    mmu_stats_t* stats = calloc(1, sizeof(mmu_stats_t));
    if (stats == NULL) {
      LOG_ERROR("STATS CREATION FAILED");
      return 1;
    }

    // test_sequential?
    if (i == 0) test_locality(stats, pmem, dir, tlb);
    if (i == 1) test_thrashing(stats, pmem, dir, tlb);
    if (i == 2) test_sequential_access(stats, pmem, dir, tlb);

    page_directory_destroy(dir);
    physical_memory_destroy(pmem);
    tlb_destroy(tlb);
    free(stats);
  }
  return 0;
}