#ifndef __PAGE_H_
#define __PAGE_H_
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PAGE_PRESENT (1 << 0)
#define PAGE_WRITABLE (1 << 1)
#define PAGE_USER (1 << 2)

// Access Type (to determine if we need to set the dirty bit)
typedef enum {
  ACCESS_READ,
  ACCESS_WRITE
} access_type_t;

typedef struct {
  uint32_t present : 1;   // is this entry valid
  uint32_t writable : 1;  // Is write to page possible
  uint32_t usermode : 1;  // User mode accessible
  uint32_t accessed : 1;  // has this been accessed
  uint32_t dirty : 1;     // has this been written to
  uint32_t unused : 7;    // reserved bits
  uint32_t frame : 20;    // physical frame number (top 20 bits of physical address)
} page_table_entry_t;

typedef struct {
  page_table_entry_t entries[1024];
} page_table_t;

typedef struct {
  page_table_t* tables[1024];  // Array of pointers to page tables
} page_directory_t;

// Extract page directory index from virtual address
uint32_t get_page_dir_index(uint32_t vaddr);

// Extract page table index from virtual address
uint32_t get_page_table_index(uint32_t vaddr);

// Extract offset from virtual address
uint32_t get_offset(uint32_t vaddr);

// Initialize a new page directory (all entries NULL)
page_directory_t* page_directory_create(void);

// Free a page directory and all its page tables
void page_directory_destroy(page_directory_t* dir);

// Allocate a page table at a specific directory index
page_table_t* page_table_create(page_directory_t* dir, uint32_t dir_index);

#endif