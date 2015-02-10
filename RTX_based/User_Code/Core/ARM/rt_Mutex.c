/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_MUTEX.C
 *      Purpose: Implements mutex synchronization objects
 *      Rev.:    V4.13
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2010 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_List.h"
#include "rt_Task.h"
#include "rt_Mutex.h"


/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/

/*--------------------------- os_mut_init -----------------------------------*/

void os_mut_init (OS_ID mutex) {
  /* Initialize a mutex object */
  P_MUCB p_MCB = mutex;

  if (os_runtask) {
    tsk_lock();
  }
  p_MCB->cb_type = MUCB;
  p_MCB->prio    = 0;
  p_MCB->level   = 0;
  p_MCB->p_lnk   = NULL;
  p_MCB->owner   = NULL;
  if (os_runtask) {
    tsk_unlock();
  }
}


/*--------------------------- os_mut_release --------------------------------*/

OS_RESULT os_mut_release (OS_ID mutex) {
  /* Release a mutex object */
  P_MUCB p_MCB = mutex;
  P_TCB p_TCB;

  tsk_lock();
  if (p_MCB->level == 0 || p_MCB->owner != os_runtask) {
    /* Unbalanced mutex release or task is not the owner */
    tsk_unlock ();
    return (OS_R_NOK);
  }
  if (--p_MCB->level != 0) {
    goto ok;
  }
  /* Restore owner task's priority. */
  os_runtask->prio = p_MCB->prio;
  if (p_MCB->p_lnk != NULL) {
    /* A task is waiting for this mutex. */
    p_TCB = os_get_first ((P_XCB)p_MCB);
    p_TCB->ret_val = OS_R_MUT;
    os_rmv_dly (p_TCB);
    /* A waiting task becomes the owner of this mutex. */
    p_MCB->level     = 1;
    p_MCB->owner     = p_TCB;
    p_MCB->prio      = p_TCB->prio;
    /* Priority inversion, check which task continues. */
    if (os_runtask->prio >= os_rdy_prio()) {
      os_dispatch (p_TCB);
    }
    else {
      /* Ready task has higher priority than running task. */
      os_put_prio (&os_rdy, os_runtask);
      os_put_prio (&os_rdy, p_TCB);
      os_runtask->state = READY;
      p_TCB->state      = READY;
      os_dispatch (NULL);
    }
  }
  else {
    /* Check if own priority raised by priority inversion. */
    if (os_rdy_prio() > os_runtask->prio) {
      os_put_prio (&os_rdy, os_runtask);
      os_runtask->state = READY;
      os_dispatch (NULL);
    }
  }
ok:tsk_unlock();
  return (OS_R_OK);
}


/*--------------------------- os_mut_wait -----------------------------------*/

OS_RESULT os_mut_wait (OS_ID mutex, U16 timeout) {
  /* Wait for a mutex, continue when mutex is free. */
  P_MUCB p_MCB = mutex;

  tsk_lock();
  if (p_MCB->level == 0) {
    p_MCB->owner = os_runtask;
    p_MCB->prio  = os_runtask->prio;
    goto inc;
  }
  if (p_MCB->owner == os_runtask) {
    /* OK, running task is the owner of this mutex. */
inc:p_MCB->level++;
    tsk_unlock();
    return (OS_R_OK);
  }
  /* Mutex owned by another task, wait until released. */
  if (timeout == 0) {
    goto rto;
  }
  /* Raise the owner task priority if lower than current priority. */
  /* This priority inversion is called priority inheritance.       */
  if (p_MCB->prio < os_runtask->prio) {
    p_MCB->owner->prio = os_runtask->prio;
    os_resort_prio (p_MCB->owner);
  }
  if (p_MCB->p_lnk != NULL) {
    os_put_prio ((P_XCB)p_MCB, os_runtask);
  }
  else {
    p_MCB->p_lnk = os_runtask;
    os_runtask->p_lnk  = NULL;
    os_runtask->p_rlnk = (P_TCB)p_MCB;
  }
  if (os_block(timeout, WAIT_MUT) == OS_R_TMO) {
rto:tsk_unlock();
    return (OS_R_TMO);
  }
  /* A mutex has been released. */
  tsk_unlock();
  return (OS_R_MUT);
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

