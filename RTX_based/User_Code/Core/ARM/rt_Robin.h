/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_ROBIN.H
 *      Purpose: Round Robin Task switching definitions
 *      Rev.:    V4.13
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2010 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

/* Variables */
extern struct OS_ROBIN os_robin;

/* Functions */
extern void os_init_robin (void);
extern void os_chk_robin  (void);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

