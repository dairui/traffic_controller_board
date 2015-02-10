/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_MAILBOX.H
 *      Purpose: Implements waits and wake-ups for mailbox messages
 *      Rev.:    V4.13
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2010 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

/* Functions */
extern void      os_mbx_init  (OS_ID mailbox, U16 mbx_size);
extern OS_RESULT os_mbx_send  (OS_ID mailbox, void *p_msg, U16 timeout);
extern OS_RESULT os_mbx_wait  (OS_ID mailbox, void **message, U16 timeout);
extern OS_RESULT os_mbx_check (OS_ID mailbox);
extern void      isr_mbx_send (OS_ID mailbox, void *p_msg);
extern OS_RESULT isr_mbx_receive (OS_ID mailbox, void **message);
extern void      os_mbx_psh   (P_MCB p_CB, void *p_msg);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

