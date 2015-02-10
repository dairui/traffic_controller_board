/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_SYSTEM.H
 *      Purpose: System Task Manager definitions
 *      Rev.:    V4.13
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2010 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

/* Variables */
#define os_psq  ((P_PSQ)&os_fifo)
extern struct OS_PSH os_psh;

/* Functions */
extern void os_psh_req (void);
extern void os_sys_manager (U32 stack);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

