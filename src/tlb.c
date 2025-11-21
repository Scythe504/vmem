#include "tlb.h"

tlb_t* tlb_create(void) {
  tlb_t* tlb = calloc(1, sizeof(tlb_t));
  if (tlb == NULL) {
    LOG_ERROR("Failed to create Translation Lookaside Buffer");
    return NULL;
  }

  tlb->victim_index = 0;
  return tlb;
}

void tlb_destroy(tlb_t* tlb) {
  if (tlb == NULL) {
    LOG_ERROR("Failed to free already NULL tlb");
    return;
  }

  free(tlb);
}

int tlb_lookup(tlb_t* tlb, uint32_t vaddr, tlb_entry_t* out_entry) {
  if (tlb == NULL) {
    LOG_ERROR("Trying to lookup a NULL tlb");
    return 0;
  }
  uint32_t tag = VPN_TAG(vaddr);

  for (unsigned i = 0; i < TLB_SIZE; i++) {
    if (tlb->entries[i].vpn_tag == tag && tlb->entries[i].valid == 1) {
      *out_entry = tlb->entries[i];
      return 1;
    }
  }

  return 0;
}

void tlb_insert(tlb_t* tlb, uint32_t vaddr, page_table_entry_t* pte) {
  if (tlb == NULL) {
    LOG_ERROR("Trying to insert entry in a NULL tlb");
    return;
  }
  uint32_t tag = VPN_TAG(vaddr);
  uint8_t victim_index = ((tlb->victim_index) % TLB_SIZE);
  if (pte->present == 0) {
    LOG_ERROR("Attempting to insert a non-present (swapped) page into TLB.");
    return;
  }
  tlb->entries[victim_index].accessed = pte->accessed;
  tlb->entries[victim_index].dirty = pte->dirty;
  tlb->entries[victim_index].pfn_value = pte->frame;
  tlb->entries[victim_index].valid = 1;
  tlb->entries[victim_index].writable = pte->writable;
  tlb->entries[victim_index].usermode = pte->usermode;
  tlb->entries[victim_index].vpn_tag = tag;

  tlb->victim_index = ((victim_index + 1) % TLB_SIZE);
}

void tlb_invalidate(tlb_t* tlb, uint32_t vaddr) {
  if (tlb == NULL) {
    LOG_ERROR("Trying to invalid a NULL tlb entry");
    return;
  }
  uint32_t tag = VPN_TAG(vaddr);

  for (unsigned i = 0; i < TLB_SIZE; i++) {
    if (tlb->entries[i].vpn_tag == tag) {
      tlb->entries[i].valid = 0;
    }
  }
}

void tlb_flush(tlb_t* tlb) {
  if (tlb == NULL) {
    LOG_ERROR("Trying to flush a NULL tlb");
    return;
  }

  for (unsigned i = 0; i < TLB_SIZE; i++) {
    tlb->entries[i].valid = 0;
  }
}
