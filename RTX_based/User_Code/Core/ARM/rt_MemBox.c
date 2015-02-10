/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_MEMBOX.C
 *      Purpose: Interface functions for fixed memory block management system
 *      Rev.:    V4.13
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2010 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_MemBox.h"

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/


/*--------------------------- _init_box -------------------------------------*/

int _init_box  (void *box_mem, U32 box_size, U32 blk_size) {
  /* Initialize memory block system, returns 0 if OK, 1 if fails. */
  void *end;
  void *blk;
  void *next;
  U32  sizeof_bm;

  /* Create memory structure. */
  if (blk_size & BOX_ALIGN_8) {
    /* Memory blocks 8-byte aligned. */ 
    blk_size = ((blk_size & ~BOX_ALIGN_8) + 7) & ~7;
    sizeof_bm = (sizeof (struct OS_BM) + 7) & ~7;
  }
  else {
    /* Memory blocks 4-byte aligned. */
    blk_size = (blk_size + 3) & ~3;
    sizeof_bm = sizeof (struct OS_BM);
  }
  if (blk_size == 0) {
    return (1);
  }
  if ((blk_size + sizeof_bm) > box_size) {
    return (1);
  }
  /* Create a Memory structure. */
  blk = ((U8 *) box_mem) + sizeof_bm;
  ((P_BM) box_mem)->free = blk;
  end = ((U8 *) box_mem) + box_size;
  ((P_BM) box_mem)->end      = end;
  ((P_BM) box_mem)->blk_size = blk_size;

  /* Link all free blocks using offsets. */
  end = ((U8 *) end) - blk_size;
  while (1)  {
    next = ((U8 *) blk) + blk_size;
    if (next > end)  break;
    *((void **)blk) = next;
    blk = next;
  }
  /* end marker */
  *((void **)blk) = 0;
  return (0);
}


/*--------------------------- _alloc_box ------------------------------------*/

void  __swi(1) *_alloc_box (void *box_mem);
void *__SWI_1              (void *box_mem) {
  /* Allocate a memory block and return start address. */
  void **free;

  free = ((P_BM) box_mem)->free;
  if (free) {
    ((P_BM) box_mem)->free = *free;
  }
  return (free);
}


/*--------------------------- _calloc_box -----------------------------------*/

void *_calloc_box (void *box_mem)  {
  /* Allocate a memory block, set it to 0, and return start address. */
  void *free;
  U32 *p;
  U32 i;

  free = _alloc_box (box_mem);
  if (free)  {
    p = free;
    for (i = ((P_BM) box_mem)->blk_size; i; i -= 4)  {
      *p = 0;
      p++;
    }
  }
  return (free);
}


/*--------------------------- _free_box -------------------------------------*/

int __swi(2) _free_box (void *box_mem, void *box);
int __SWI_2            (void *box_mem, void *box) {
  /* Free a memory block, returns 0 if OK, 1 if box does not belong to box_mem */
  if (box < box_mem) {
    return (1);
  }
  if (box > ((P_BM) box_mem)->end) {
    return (1);
  }
  *((void **)box) = ((P_BM) box_mem)->free;
  ((P_BM) box_mem)->free = box;
  return (0);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

