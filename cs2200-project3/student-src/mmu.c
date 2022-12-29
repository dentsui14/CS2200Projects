#include "mmu.h"
#include "pagesim.h"
#include "va_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* The frame table pointer. You will set this up in system_init. */
fte_t *frame_table;

/**
 * --------------------------------- PROBLEM 2 --------------------------------------
 * Checkout PDF sections 4 for this problem
 * 
 * In this problem, you will initialize the frame_table pointer. The frame table will
 * be located at physical address 0 in our simulated memory. You should zero out the 
 * entries in the frame table, in case for any reason physical memory is not clean.
 * 
 * HINTS:
 *      - mem: Simulated physical memory already allocated for you.
 *      - PAGE_SIZE: The size of one page
 * ----------------------------------------------------------------------------------
 */
void system_init(void) {
    frame_table = (fte_t *) mem;
    memset(frame_table, 0, PAGE_SIZE);
    frame_table[0].protected = 1;
}

/**
 * --------------------------------- PROBLEM 5 --------------------------------------
 * Checkout PDF section 6 for this problem
 * 
 * Takes an input virtual address and performs a memory operation.
 * 
 * @param addr virtual address to be translated
 * @param access 'r' if the access is a read, 'w' if a write
 * @param data If the access is a write, one byte of data to written to our memory.
 *             Otherwise NULL for read accesses.
 * 
 * HINTS:
 *      - Remember that not all the entry in the process's page table are mapped in. 
 *      Check what in the pte_t struct signals that the entry is mapped in memory.
 * ----------------------------------------------------------------------------------
 */
uint8_t mem_access(vaddr_t addr, char access, uint8_t data) {
    // TODO: translate virtual address to physical, then perform the specified operation
    vpn_t vpn = vaddr_vpn(addr);
    uint16_t offset = vaddr_offset(addr);
    pte_t* page_table = (pte_t*)(mem + (PTBR * PAGE_SIZE)); //since we are using a ptbr value we don't have to check to make sure page table is valid
    if (page_table[vpn].valid == 0) {
        page_fault(addr);
    }
    pfn_t pfn = page_table[vpn].pfn;
    frame_table[pfn].referenced = 1;
    paddr_t phys_addr = (paddr_t)((pfn << OFFSET_LEN)) | offset;
    stats.accesses++;
    /* Either read or write the data to the physical address
       depending on 'rw' */
    if (access == 'r') {
        return mem[phys_addr];
    } else {
        page_table[vpn].dirty = 1;
        mem[phys_addr] = data;
    }

    return 0;
}