/*
 * Copyright (C) 2026 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* LamiaAtrium release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

//#ifdef MM_PAGING
/*
 * PAGING based Memory Management
 * Virtual memory module mm/mm-vm.c
 */

#include "string.h"
#include "mm.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*get_vma_by_num - get vm area by numID
 *@mm: memory region
 *@vmaid: ID vm area to alloc memory region
 *
 */
struct vm_area_struct *get_vma_by_num(struct mm_struct *mm, int vmaid)
{
  struct vm_area_struct *pvma = mm->mmap;

  if (mm->mmap == NULL)
    return NULL;

  int vmait = pvma->vm_id;

  while (vmait < vmaid)
  {
    if (pvma == NULL)
      return NULL;

    pvma = pvma->vm_next;
    vmait = pvma->vm_id;
  }

  return pvma;
}

int __mm_swap_page(struct pcb_t *caller, addr_t vicfpn , addr_t swpfpn)
{
    __swap_cp_page(caller->krnl->mram, vicfpn, caller->krnl->active_mswp, swpfpn);
    return 0;
}

/*get_vm_area_node - get vm area for a number of pages
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@incpgnum: number of page
 *@vmastart: vma end
 *@vmaend: vma end
 *
 */
struct vm_rg_struct *get_vm_area_node_at_brk(struct pcb_t *caller, int vmaid, addr_t size, addr_t alignedsz)
{
  struct vm_rg_struct * newrg;
  /* TODO retrive current vma to obtain newrg, current comment out due to compiler redundant warning*/
  struct vm_area_struct *cur_vma = get_vma_by_num(caller->krnl->mm, vmaid);

  newrg = malloc(sizeof(struct vm_rg_struct));

  /* TODO: update the newrg boundary
  // newrg->rg_start = ...
  // newrg->rg_end = ...
  */
  
  newrg->rg_start = cur_vma->sbrk;
  newrg->rg_end = newrg->rg_start + size;
  newrg->rg_next = NULL;

  /* END TODO */

  return newrg;
}

/*validate_overlap_vm_area
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@vmastart: vma end
 *@vmaend: vma end
 *
 */
int validate_overlap_vm_area(struct pcb_t *caller, int vmaid, addr_t vmastart, addr_t vmaend)
{
  //struct vm_area_struct *vma = caller->krnl->mm->mmap;

  /* TODO validate the planned memory area is not overlapped */
  if (vmastart >= vmaend)
  {
    return -1;
  }

  struct vm_area_struct *vma = caller->krnl->mm->mmap;
  /* Iterate all VM Areas to check overlap */
  while (vma != NULL)
  { 
    /* We only check overlap with OTHER areas, not the current one we are expanding */
    if (vma->vm_id != vmaid) 
    {
        if (OVERLAP(vmastart, vmaend, vma->vm_start, vma->vm_end))
        {
            return -1;
        }
    }
    vma = vma->vm_next;
  }
  
  /* End TODO*/

  return 0;
}

/*inc_vma_limit - increase vm area limits to reserve space for new variable
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@inc_sz: increment size
 *
 */
int inc_vma_limit(struct pcb_t *caller, int vmaid, addr_t inc_sz)
{
  struct vm_rg_struct * newrg = malloc(sizeof(struct vm_rg_struct));
  int inc_amt;
  int incnumpage;
  struct vm_area_struct *cur_vma = get_vma_by_num(caller->krnl->mm, vmaid);
  int old_sbrk;

  /* TOTO with new address scheme, the size need tobe aligned 
   * the raw inc_sz maybe not fit pagesize
   */ 
  inc_amt = PAGING_PAGE_ALIGNSZ(inc_sz);
  incnumpage =  inc_amt / PAGING_PAGESZ;

  /* TODO Validate overlap of obtained region */
  old_sbrk = cur_vma->sbrk;

  /* Check if expanding sbrk by inc_amt overlaps any other area */
  if (validate_overlap_vm_area(caller, vmaid, old_sbrk, old_sbrk + inc_amt) < 0)
  {
    return -1; /*Overlap and failed allocation */
  }

  /* TODO: Obtain the new vm area based on vmaid */
  /* newrg was malloc'd at top */
  newrg->rg_start = old_sbrk;
  newrg->rg_end = old_sbrk + inc_sz; // Actual usage size
  newrg->rg_next = NULL;

  //cur_vma->vm_end... 
  // inc_limit_ret...
  
  /* The obtained vm area (only)
   * now will be alloc real ram region */
  
  /* Map the memory to MEMRAM - This creates PTEs and allocates frames */
  if (vm_map_ram(caller, old_sbrk, old_sbrk + inc_amt, 
                    old_sbrk, incnumpage , newrg) < 0)
  {
    free(newrg);
    return -1; /* Map the memory to MEMRAM failed */
  }

  /* Successful mapping, now update the VMA limits */
  cur_vma->sbrk += inc_amt;
  /* Usually vm_end is the hard limit, but in this simulation sbrk expands the area */
  if (cur_vma->sbrk > cur_vma->vm_end) {
      cur_vma->vm_end = cur_vma->sbrk;
  }

  return 0;
}

// #endif