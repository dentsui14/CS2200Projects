#include "mmu.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 6 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Page fault handler.
 * 
 * When the CPU encounters an invalid address mapping in a page table, it invokes the 
 * OS via this handler. Your job is to put a mapping in place so that the translation 
 * can succeed.
 * 
 * @param addr virtual address in the page that needs to be mapped into main memory.
 * 
 * HINTS:
 *      - You will need to use the global variable current_process when
 *      altering the frame table entry.
 *      - Use swap_exists() and swap_read() to update the data in the 
 *      frame as it is mapped in.
 * ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t addr) {
   // TODO: Get a new frame, then correctly update the page table and frame table
   vpn_t process_vpn = vaddr_vpn(addr);
   //uint16_t process_offset = vaddr_offset(addr);
   pte_t* page_table_entry = (pte_t*)(mem + (PTBR * PAGE_SIZE)) + process_vpn;
   pfn_t new_pfn = free_frame();
   void* new_frame = mem + (new_pfn * PAGE_SIZE);
   page_table_entry->pfn = new_pfn;
   page_table_entry->valid = 1;
   page_table_entry->dirty = 0;

   frame_table[new_pfn].mapped = 1;
   frame_table[new_pfn].protected = 0;
   frame_table[new_pfn].referenced = 1; //maybe remove
   frame_table[new_pfn].process = current_process;
   frame_table[new_pfn].vpn = process_vpn;
   stats.page_faults++;

   if(swap_exists(page_table_entry)) {
      swap_read(page_table_entry, new_frame);
   } else {
      memset(new_frame, 0, PAGE_SIZE);
   }
}

#pragma GCC diagnostic pop
