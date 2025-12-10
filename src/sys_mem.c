/*
 * Copyright (C) 2026 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* LamiaAtrium release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

#include "os-mm.h"
#include "syscall.h"
#include "libmem.h"
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef MM64
#include "mm64.h"
#else
#include "mm.h"
#endif

//typedef char BYTE;

int __sys_memmap(struct krnl_t *krnl, uint32_t pid, struct sc_regs* regs)
{
   int memop = regs->a1;
   BYTE value;
   
   /* TODO THIS DUMMY CREATE EMPTY PROC TO AVOID COMPILER NOTIFY 
    * need to be eliminated
    */
   // struct pcb_t *caller = malloc(sizeof(struct pcb_t)); // <-- REMOVED DUMMY

   struct pcb_t *caller = NULL;

   /*
    * @bksysnet: Please note in the dual spacing design
    * syscall implementations are in kernel space.
    */

   /* TODO: Traverse proclist to terminate the proc
    * stcmp to check the process match proc_name
    */
   struct queue_t *running_list = krnl->running_list;

    /* TODO Maching and marking the process */
    /* user process are not allowed to access directly pcb in kernel space of syscall */
    
    // Logic tìm PCB từ PID trong running_list
    if (running_list != NULL) {
        for (int i = 0; i < running_list->size; i++) {
            if (running_list->proc[i] != NULL && running_list->proc[i]->pid == pid) {
                caller = running_list->proc[i];
                break;
            }
        }
    }

    if (caller == NULL) {
        // Không tìm thấy process với pid tương ứng
        return -1;
    }
    
   switch (memop) {
   case SYSMEM_MAP_OP:
            /* Reserved process case*/
            vmap_pgd_memset(caller, regs->a2, regs->a3);
            break;
   case SYSMEM_INC_OP:
            inc_vma_limit(caller, regs->a2, regs->a3);
            break;
   case SYSMEM_SWP_OP:
            __mm_swap_page(caller, regs->a2, regs->a3);
            break;
   case SYSMEM_IO_READ:
            // Đọc từ Physical Memory (MRAM)
            // Lưu ý: caller->krnl->mram phải trỏ đúng đến mram hệ thống
            if (MEMPHY_read(krnl->mram, regs->a2, &value) == 0) {
               regs->a3 = value; // Trả giá trị đọc được về thanh ghi a3
            } else {
               regs->a3 = 0; // Hoặc mã lỗi
            }
            break;
   case SYSMEM_IO_WRITE:
            // Ghi vào Physical Memory (MRAM)
            MEMPHY_write(krnl->mram, regs->a2, regs->a3);
            break;
   default:
            printf("Memop code: %d\n", memop);
            break;
   }
   
   return 0;
}