#ifndef __TLB_H_
#define __TLB_H_
#include <stdint.h>

#include "logger.h"
#include "page.h"
#define TLB_SIZE 32

#define VPN_TAG(vaddr) ((vaddr >> 12) & 0xFFFFF)

typedef struct {
  // The 20-bit Virtual Page Number (VPN)
  uint32_t vpn_tag;

  // The 20-bit Physical Frame Number (PFN)
  uint32_t pfn_value;

  // A single bit can use a full uint8_t for clarity and alignment
  uint8_t valid;     // 1: Entry contains a valid mapping
  uint8_t writable;  // 1: Write access is permitted
  uint8_t usermode;  // 1: Accessible by user level
  uint8_t accessed;  // 1: Has been recently accessed
  uint8_t dirty;     // 1: Page has been written to

} tlb_entry_t;

typedef struct {
  tlb_entry_t entries[TLB_SIZE];  // contains tlb_entry which has the top bits (vaddr) and specific frame number or physical page address
  uint8_t victim_index;           // points to the next tlb entry to be replaced
} tlb_t;

tlb_t* tlb_create(void);
void tlb_destroy(tlb_t* tlb);

// Look up a virtual page in the TLB
// Returns index if hit (and fills out_entry), -1 if miss
int tlb_lookup(tlb_t* tlb, uint32_t vaddr);

// Insert a translation into the TLB
void tlb_insert(tlb_t* tlb, uint32_t vaddr, page_table_entry_t* pte);

// Invalidate a specific TLB entry
void tlb_invalidate(tlb_t* tlb, uint32_t vaddr);

// Flush entire TLB
void tlb_flush(tlb_t* tlb);

// Updates write flag in the tlb entry
void tlb_update_flags(tlb_t* tlb, unsigned index, uint8_t accessed, uint8_t dirty);

#endif