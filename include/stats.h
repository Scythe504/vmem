#ifndef __STATS_H_
#define __STATS_H_

#include <stdint.h>
#include <stdio.h>

#include "page.h"
#include "pmem.h"
#include "tlb.h"

typedef struct {
  uint64_t tlb_hits;
  uint64_t tlb_misses;
  uint64_t page_faults;
  uint64_t demand_page_allocs;
  uint64_t pages_swapped_in;
  uint64_t pages_swapped_out;
  uint64_t total_translations;
  uint64_t page_table_walks;
} mmu_stats_t;

void test_sequential_access(
    mmu_stats_t* stats,
    physical_memory_t* pmem,
    page_directory_t* dir,
    tlb_t* tlb);
void test_thrashing(
    mmu_stats_t* stats,
    physical_memory_t* pmem,
    page_directory_t* dir,
    tlb_t* tlb);
void test_locality(
    mmu_stats_t* stats,
    physical_memory_t* pmem,
    page_directory_t* dir,
    tlb_t* tlb);

void test_memory_exhaustion(
    mmu_stats_t* stats,
    physical_memory_t* pmem,
    page_directory_t* dir,
    tlb_t* tlb);

void test_dirty_pages(
    mmu_stats_t* stats,
    physical_memory_t* pmem,
    page_directory_t* dir,
    tlb_t* tlb);
#endif