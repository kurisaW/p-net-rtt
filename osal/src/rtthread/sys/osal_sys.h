/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2017 rt-labs AB, Sweden.
 *
 * This software is licensed under the terms of the BSD 3-clause
 * license. See the file LICENSE distributed with this software for
 * full license information.
 ********************************************************************/

#ifndef OSAL_SYS_H
#define OSAL_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>

#define OS_THREAD
#define OS_MUTEX
#define OS_SEM
#define OS_EVENT
#define OS_MBOX
#define OS_TIMER
#define OS_TICK

#define OS_MUTEX_NAME "pmut"
#define OS_SEM_NAME "psem"
#define OS_EVENT_NAME "pev"
#define OS_MBOX_NAME "pmbx"
#define OS_TIMER_NAME "ptmr"

typedef rt_thread_t os_thread_t;
typedef rt_mutex_t os_mutex_t;
typedef rt_sem_t os_sem_t;
typedef rt_event_t os_event_t;
typedef rt_mailbox_t os_mbox_t;

typedef struct os_timer
{
   rt_timer_t handle;
   void(*fn) (struct os_timer *, void * arg);
   void * arg;
   uint32_t us;
} os_timer_t;

typedef rt_tick_t os_tick_t;

#ifdef __cplusplus
}
#endif

#endif /* OSAL_SYS_H */
