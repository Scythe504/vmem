#ifndef __TRANSLATE_H_
#define __TRANSLATE_H_

#include <stdint.h>

#include "logger.h"
#include "page.h"
#include "pmem.h"
#include "stats.h"
#include "tlb.h"

// Map a virtual address to a physical frame
// Returns 0 on success, -1 on failure
int map_virtual_to_physical(
    page_directory_t* dir,
    physical_memory_t* pmem,
    uint32_t vaddr,
    uint32_t flags,  // e.g., WRITABLE | USER
    tlb_t* tlb);

// Create a lazy mapping (doesn't allocate physical memory yet)
int map_virtual_lazy(
    page_directory_t* dir,
    uint32_t vaddr,
    uint32_t flags);

// Translate virtual address to physical address
// Returns physical address, or -1 if not mapped
int32_t translate_address(
    page_directory_t* dir,
    uint32_t vaddr,
    access_type_t type,
    uint8_t curr_privilege,
    physical_memory_t* pmem,
    tlb_t* tlb,
    mmu_stats_t* stats);

// Page fault handler - called when translate_address finds present=0
int handle_page_fault(
    page_directory_t* dir,
    physical_memory_t* pmem,
    tlb_t* tlb,
    uint32_t vaddr,
    mmu_stats_t* stats);

int evict_page(page_directory_t* dir, tlb_t* tlb, uint32_t vaddr_to_evict);

#endif