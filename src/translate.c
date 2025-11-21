#include "translate.h"

int map_virtual_to_physical(page_directory_t* dir, physical_memory_t* pmem, uint32_t vaddr, uint32_t flags) {
  int32_t page_dir_index = get_page_dir_index(vaddr);
  page_table_t* page_table = dir->tables[page_dir_index];

  if (page_table == NULL) {
    page_table = page_table_create(dir, page_dir_index);
  }

  // check if page_table_entry is already mapped (present bit == 0)
  uint32_t page_table_index = get_page_table_index(vaddr);

    page_table_entry_t page_table_entry = page_table->entries[page_table_index];

    if (page_table_entry.present == 1) {
      // table_entry has already been mapped to a physical addr
      return -1;
    }

    int32_t frame_number = physical_frame_alloc(pmem);
    if (frame_number < 0) {
      return -1;
    }
    page_table_entry.usermode = flags;
    page_table_entry.present = 1; // entry has been mapped
    page_table_entry.frame = frame_number;

    page_table->entries[page_table_index] = page_table_entry;
    return 0;
}

int32_t translate_address(page_directory_t* dir, uint32_t vaddr) {
  // Extract indices
  uint32_t page_dir_index = get_page_dir_index(vaddr);
  uint32_t page_table_index = get_page_table_index(vaddr);
  uint32_t offset = get_offset(vaddr);

  page_table_t* page_table = dir->tables[page_dir_index];

  if (page_table == NULL) {
    return -1;
  }

  page_table_entry_t page_table_entry = page_table->entries[page_table_index];
  if (!page_table_entry.present) {
    return -1;
  }

  uint32_t frame_number = page_table_entry.frame; 

  // frame_number 20 bits | offset 12 bits
  uint32_t physical_addr = (frame_number << 12) | offset;

  return physical_addr;
}
