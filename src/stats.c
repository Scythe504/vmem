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
  printf("DEMAND_ALLOCS: %" PRIu64 "\n", stats->demand_page_allocs);
  printf("SWAP_IN: %" PRIu64 "\n", stats->pages_swapped_in);
  printf("SWAP_OUT: %" PRIu64 "\n", stats->pages_swapped_out);
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
  for (unsigned i = 0; i < 1000; i++) {
    uint32_t vaddr = i * PAGE_SIZE_BYTES;
    uint32_t flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;

    // int result = map_virtual_to_physical(dir, pmem, vaddr, flags, tlb);
    int result = map_virtual_lazy(dir, vaddr, flags);
    if (result < 0) {
      printf("Failed to map vaddr 0x%08x\n", vaddr);
    }
  }

  // Access each page sequentially TWICE
  for (int pass = 0; pass < 2; pass++) {
    for (unsigned i = 0; i < 1000; i++) {
      uint32_t vaddr = i * PAGE_SIZE_BYTES;
      translate_address(dir, vaddr, ACCESS_READ, 1, pmem, tlb, stats);
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
    // map_virtual_to_physical(dir, pmem, vaddr, flags, tlb);
    map_virtual_lazy(dir, vaddr, flags);
  }

  // Access all 64 pages repeatedly in order
  for (int round = 0; round < 10; round++) {
    for (unsigned i = 0; i < 64; i++) {
      uint32_t vaddr = i * PAGE_SIZE_BYTES;
      translate_address(dir, vaddr, ACCESS_READ, 1, pmem, tlb, stats);
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
  for (unsigned i = 0; i < 1000; i++) {
    uint32_t vaddr = i * PAGE_SIZE_BYTES;
    uint32_t flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    map_virtual_lazy(dir, vaddr, flags);
  }

  // Access them 100 times with good locality
  // (repeatedly accessing the same small set)
  for (int i = 0; i < 100; i++) {
    uint32_t page_num = i % 10;  // Cycle through 0-9
    uint32_t vaddr = page_num * PAGE_SIZE_BYTES;
    translate_address(dir, vaddr, ACCESS_READ, 1, pmem, tlb, stats);
  }

  print_stats(stats, "GOOD LOCALITY");
}

void test_memory_exhaustion(
    mmu_stats_t* stats,
    physical_memory_t* pmem,
    page_directory_t* dir,
    tlb_t* tlb) {
  // Lazily map 100 pages
  for (unsigned i = 0; i < 100; i++) {
    // YOUR CODE: map_virtual_lazy...
  }

  // Access all 100 pages
  for (unsigned i = 0; i < 100; i++) {
    // YOUR CODE: translate_address...
  }

  print_stats(stats, "MEMORY EXHAUSTION (100 pages, limited physical memory)");

  // QUESTION FOR YOU: What should SWAP_OUT be?
  // Write your prediction here: _______
}

void test_dirty_pages(
    mmu_stats_t* stats,
    physical_memory_t* pmem,
    page_directory_t* dir,
    tlb_t* tlb) {
    
    // Map 20 pages lazily
    // YOUR CODE
    
    // WRITE to first 10 pages (makes them dirty)
    for (unsigned i = 0; i < 10; i++) {
        // YOUR CODE: translate with ACCESS_WRITE
    }
    
    // READ from next 10 pages
    for (unsigned i = 10; i < 20; i++) {
        // YOUR CODE: translate with ACCESS_READ
    }
    
    print_stats(stats, "DIRTY PAGES TEST");
    
    // QUESTION: How many pages are dirty? How can you tell?
}