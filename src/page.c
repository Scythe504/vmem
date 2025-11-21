#include "page.h"
#include "logger.h"

// Extract page directory index from virtual address
uint32_t get_page_dir_index(uint32_t vaddr) {
  // Extract left 10 bits
  uint32_t top_bits = vaddr >> 22;
  return top_bits;
}

// Extract page table index from virtual address
uint32_t get_page_table_index(uint32_t vaddr) {
  // Extract middle 10 bits
  uint32_t middle_bits = (vaddr >> 12) & 0x3FF;
  return middle_bits;
}

// Extract offset from virtual address
uint32_t get_offset(uint32_t vaddr) {
  // Extract right 12 bits
  uint32_t bottom_bits = (vaddr & 0xFFF);
  return bottom_bits;
}

// Initializes a new page directory (all entries NULL)
page_directory_t* page_directory_create(void) {
  page_directory_t* dir = calloc(1, sizeof(page_directory_t));

  // check if allocation succeeded
  if (dir == NULL) {
    LOG_ERROR("Failed to allocate %zu bytes for the Page Directory.",
              sizeof(page_directory_t));
    return NULL;
  }

  return dir;
}

// Frees a page directory and all its page tables
void page_directory_destroy(page_directory_t* dir) {
  if (dir == NULL) {
    LOG_ERROR("Failed to free already NULL dir.");
    return;
  }

  // check if tables pointer points to something
  for (int i = 0; i < 1024; i++) {
    if (dir->tables[i] != NULL) {
      free(dir->tables[i]);
      dir->tables[i] = NULL;
    }
  }

  // free the page_directory
  free(dir);
}

// Allocate a page table at a specific directory index
page_table_t* page_table_create(page_directory_t* dir, uint32_t dir_index) {
  if (dir == NULL || dir_index >= 1024) {  // Add bounds check
    LOG_ERROR("Invalid directory pointer or index: %u", dir_index);
    return NULL;
  }

  if (dir->tables[dir_index] != NULL) {
    LOG_ERROR("Dir:%x, Idx:%u, Already have memory allocated", dir, dir_index);
    return dir->tables[dir_index];
  }

  page_table_t* new_table_ptr = calloc(1, sizeof(page_table_t));

  if (new_table_ptr == NULL) {
    LOG_ERROR("Dir:%x, Idx:%u, Failed to allocate memory", dir, dir_index);
    return NULL;
  }

  dir->tables[dir_index] = new_table_ptr;

  return new_table_ptr;
}
