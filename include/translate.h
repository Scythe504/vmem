#ifndef __TRANSLATE_H_
#define __TRANSLATE_H_
#include "pmem.h"
#include "stdint.h"
#include "page.h"
#include "logger.h"
// Map a virtual address to a physical frame
// Returns 0 on success, -1 on failure
int map_virtual_to_physical(
    page_directory_t* dir,
    physical_memory_t* pmem,
    uint32_t vaddr,
    uint32_t flags  // e.g., WRITABLE | USER
);

// Translate virtual address to physical address
// Returns physical address, or -1 if not mapped
int32_t translate_address(
    page_directory_t* dir,
    uint32_t vaddr);

#endif