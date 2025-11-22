#include "translate.h"

int map_virtual_to_physical(
    page_directory_t* dir,
    physical_memory_t* pmem,
    uint32_t vaddr,
    uint32_t flags,
    tlb_t* tlb) {
  int32_t page_dir_index = get_page_dir_index(vaddr);
  page_table_t* page_table = dir->tables[page_dir_index];

  if (page_table == NULL) {
    page_table = page_table_create(dir, page_dir_index);

    if (page_table == NULL) {
      return -1;
    }
  }

  // check if page_table_entry is already mapped (present bit == 0)
  uint32_t page_table_index = get_page_table_index(vaddr);

  page_table_entry_t* page_table_entry = &page_table->entries[page_table_index];

  if (page_table_entry->present == 1) {
    // table_entry has already been mapped to a physical addr
    return -1;
  }

  int32_t frame_number = physical_frame_alloc(pmem);
  if (frame_number < 0) {
    return -1;
  }

  page_table_entry->writable = (flags & PAGE_WRITABLE) ? 1 : 0;
  page_table_entry->usermode = (flags & PAGE_USER) ? 1 : 0;
  page_table_entry->present = (flags & PAGE_PRESENT) ? 1 : 0;
  page_table_entry->frame = frame_number;
  page_table_entry->accessed = 0;  // Not accessed yet
  page_table_entry->dirty = 0;     // Not modified yet

  tlb_invalidate(tlb, vaddr);

  return 0;
}

int32_t translate_address(
    page_directory_t* dir,
    uint32_t vaddr,
    access_type_t type,
    uint8_t curr_privilege,
    tlb_t* tlb,
    mmu_stats_t* stats) {
  // Extract indices
  if (stats) {
    stats->total_translations++;
  }

  uint32_t page_dir_index = get_page_dir_index(vaddr);
  uint32_t page_table_index = get_page_table_index(vaddr);
  uint32_t offset = get_offset(vaddr);

  int tlb_hit = tlb_lookup(tlb, vaddr);

  uint8_t dirty = 1;
  uint8_t accessed = 1;
  // pte found in tlb
  if (tlb_hit >= 0) {
    if (stats) stats->tlb_hits++;
    if (curr_privilege == 1 && !tlb->entries[tlb_hit].usermode) {
      printf("PAGE FAULT (TLB Hit): User-mode access attempted on Supervisor-only page.\n");
      if (stats) stats->page_faults++;
      return -1;
    }

    if (type == ACCESS_WRITE && !tlb->entries[tlb_hit].writable) {
      printf("PAGE FAULT (TLB Hit): Write attempted on Read-Only page.\n");
      if (stats) stats->page_faults++;
      return -1;
    }

    page_table_entry_t* pte = &dir->tables[page_dir_index]->entries[page_table_index];
    if (type == ACCESS_WRITE) {
      tlb_update_flags(tlb, tlb_hit, accessed, dirty);
      pte->dirty = dirty;
    }
    pte->accessed = accessed;

    uint32_t paddr = (tlb->entries[tlb_hit].pfn_value << 12) | offset;
    return paddr;
  }

  if (stats) {
    stats->tlb_misses++;
    stats->page_table_walks++;
  }
  page_table_t* page_table = dir->tables[page_dir_index];

  page_table_entry_t* page_table_entry = &page_table->entries[page_table_index];
  if (page_table == NULL || !page_table_entry->present) {
    printf("PAGE FAULT: Page Directory Entry not present %X\n", page_dir_index);
    if (stats) stats->page_faults++;
    return -1;
  }

  // Privelege and Permissions check
  if (type == ACCESS_WRITE && !page_table_entry->writable) {
    printf("PAGE FAULT: Write attempted on Read-Only page.\n");
    if (stats) stats->page_faults++;
    return -1;  // Error: Write protection fault
  }

  if (curr_privilege == 1 && !page_table_entry->usermode) {
    printf("PAGE FAULT: User-mode access attempted on Supervisor-only page.\n");
    if (stats) stats->page_faults++;
    return -1;
  }

  page_table_entry->accessed = 1;

  if (type == ACCESS_WRITE) {
    page_table_entry->dirty = dirty;
  }

  uint32_t frame_number = page_table_entry->frame;

  // frame_number 20 bits | offset 12 bits
  uint32_t physical_addr = (frame_number << 12) | offset;

  tlb_insert(tlb, vaddr, page_table_entry);

  return physical_addr;
}
