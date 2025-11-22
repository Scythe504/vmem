#ifndef __TRANSLATE_H_
#define __TRANSLATE_H_

#include <stdint.h>

#include "logger.h"
#include "page.h"
#include "pmem.h"
#include "tlb.h"
#include "stats.h"

// Map a virtual address to a physical frame
// Returns 0 on success, -1 on failure
int map_virtual_to_physical(
    page_directory_t* dir,
    physical_memory_t* pmem,
    uint32_t vaddr,
    uint32_t flags,  // e.g., WRITABLE | USER
    tlb_t* tlb);

// Translate virtual address to physical address
// Returns physical address, or -1 if not mapped
int32_t translate_address(
    page_directory_t* dir,
    uint32_t vaddr,
    access_type_t type,
    uint8_t curr_privilege,
    tlb_t* tlb,
    mmu_stats_t* stats);

#endif