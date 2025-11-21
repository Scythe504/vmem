#include <stdio.h>

#include "page.h"
#include "pmem.h"
#include "translate.h"

int main() {
  // Create structures
  page_directory_t* dir = page_directory_create();
  physical_memory_t* pmem = physical_memory_create(1024 * 1024 * 1024);  // 1GB

  // Test 1: Map a single page
  uint32_t vaddr1 = 0x00000000;
  map_virtual_to_physical(dir, pmem, vaddr1, PAGE_WRITABLE);
  int32_t paddr1 = translate_address(dir, vaddr1);

  printf("DEBUG: How many frames used? %u\n", pmem->used_frames);
  printf("Test 1: vaddr=0x%08x -> paddr=0x%08x\n", vaddr1, paddr1);

  // Test 2: Map a page far away (different page table)
  uint32_t vaddr2 = 0x7FFFF000;
  map_virtual_to_physical(dir, pmem, vaddr2, PAGE_WRITABLE);
  int32_t paddr2 = translate_address(dir, vaddr2);
  printf("DEBUG: How many frames used? %u\n", pmem->used_frames);
  printf("Test 2: vaddr=0x%08x -> paddr=0x%08x\n", vaddr2, paddr2);

  // Test 3: Try to translate an unmapped address (should fail)
  int32_t paddr3 = translate_address(dir, 0x12345000);
  printf("Test 3: Unmapped address returned: 0x%08x (should be -1)\n", paddr3);

  // Test 4: Verify offset is preserved
  uint32_t vaddr4 = 0x00000ABC;
  map_virtual_to_physical(dir, pmem, vaddr4, PAGE_WRITABLE);
  int32_t paddr4 = translate_address(dir, vaddr4);
  printf("DEBUG: How many frames used? %u\n", pmem->used_frames);
  printf("Test 4: vaddr=0x%08x -> paddr=0x%08x (offset 0xABC should be preserved)\n", vaddr4, paddr4);
  
  // Cleanup
  page_directory_destroy(dir);
  physical_memory_destroy(pmem);
}