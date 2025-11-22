#include "stats.h"

#include <inttypes.h>

#include "translate.h"

#define TLB_HIT_RATE(hit, total) (uint64_t)((hit * 100) / total)

void print_stats(mmu_stats_t* stats, char* name) {
  printf("\n===========( %s )===========\n", name);
  printf("TLB_HITS: %" PRIu64 "\n", stats->tlb_hits);
  printf("TLB_MISSES: %" PRIu64 "\n", stats->tlb_misses);
  printf("PAGE_FAULTS: %" PRIu64 "\n", stats->page_faults);
  printf("PAGE_WALKS: %" PRIu64 "\n", stats->page_table_walks);
  printf("TOTAL_TRANSLATIONS: %" PRIu64 "\n", stats->total_translations);
  printf("TLB_HIT_RATE(percent) %" PRIu64 "\n", TLB_HIT_RATE(stats->tlb_hits, stats->total_translations));
  printf("==============================================\n");
}

void test_sequential_access(
    mmu_stats_t* stats,
    physical_memory_t* pmem,
    page_directory_t* dir,
    tlb_t* tlb) {
  // Map 100 pages with proper flags
  for (unsigned i = 0; i < 100; i++) {
    uint32_t vaddr = i * PAGE_SIZE_BYTES;
    uint32_t flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;

    int result = map_virtual_to_physical(dir, pmem, vaddr, flags, tlb);
    if (result < 0) {
      printf("Failed to map vaddr 0x%08x\n", vaddr);
    }
  }

  // Access each page sequentially TWICE
  for (int pass = 0; pass < 2; pass++) {
    for (unsigned i = 0; i < 30; i++) {
      uint32_t vaddr = i * PAGE_SIZE_BYTES;
      translate_address(dir, vaddr, ACCESS_READ, 1, tlb, stats);
    }
  }

  print_stats(stats, "SEQUENTIAL (2 passes over 30 pages)");
}

void test_thrashing(
    mmu_stats_t* stats,
    physical_memory_t* pmem,
    page_directory_t* dir,
    tlb_t* tlb) {
  // Map 64 pages (twice the TLB size of 32)
  for (unsigned i = 0; i < 64; i++) {
    uint32_t vaddr = i * PAGE_SIZE_BYTES;
    uint32_t flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    map_virtual_to_physical(dir, pmem, vaddr, flags, tlb);
  }

  // Access all 64 pages repeatedly in order
  for (int round = 0; round < 10; round++) {
    for (unsigned i = 0; i < 32; i++) {
      uint32_t vaddr = i * PAGE_SIZE_BYTES;
      translate_address(dir, vaddr, ACCESS_READ, 1, tlb, stats);
    }
  }

  print_stats(stats, "TLB THRASHING (10 passes over 32 pages, TLB=32)");
}

void test_locality(
    mmu_stats_t* stats,
    physical_memory_t* pmem,
    page_directory_t* dir,
    tlb_t* tlb) {
  // Map just 10 pages
  for (unsigned i = 0; i < 10; i++) {
    uint32_t vaddr = i * PAGE_SIZE_BYTES;
    uint32_t flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    map_virtual_to_physical(dir, pmem, vaddr, flags, tlb);
  }

  // Access them 100 times with good locality
  // (repeatedly accessing the same small set)
  for (int i = 0; i < 100; i++) {
    uint32_t page_num = i % 10;  // Cycle through 0-9
    uint32_t vaddr = page_num * PAGE_SIZE_BYTES;
    translate_address(dir, vaddr, ACCESS_READ, 1, tlb, stats);
  }

  print_stats(stats, "GOOD LOCALITY (100 accesses to 10 pages)");
}