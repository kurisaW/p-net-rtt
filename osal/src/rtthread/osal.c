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

#include "osal.h"

#include <stdlib.h>

#define TMO_TO_TICKS(ms) \
   ((ms == OS_WAIT_FOREVER) ? RT_WAITING_FOREVER : (rt_int32_t)rt_tick_from_millisecond(ms))

void * os_malloc (size_t size)
{
   return malloc (size);
}

void os_free (void * ptr)
{
   free (ptr);
}

os_thread_t * os_thread_create (
   const char * name,
   uint32_t priority,
   size_t stacksize,
   void (*entry) (void * arg),
   void * arg)
{
   rt_thread_t tid = rt_thread_create(name, entry, arg, stacksize, priority, 20);
   if (tid != RT_NULL) {
      rt_thread_startup(tid);
   }

   return (os_thread_t *)tid;
}

os_mutex_t * os_mutex_create (void)
{
   static unsigned short counter = 0;
   char tname[RT_NAME_MAX];

   rt_snprintf(tname, RT_NAME_MAX, "%s%d", OS_MUTEX_NAME, counter++);

   return (os_mutex_t *)rt_mutex_create(tname, RT_IPC_FLAG_PRIO);
}

void os_mutex_lock (os_mutex_t * mutex)
{
   rt_mutex_take((rt_mutex_t)mutex, RT_WAITING_FOREVER);
}

void os_mutex_unlock (os_mutex_t * mutex)
{
   rt_mutex_release((rt_mutex_t)mutex);
}

void os_mutex_destroy (os_mutex_t * mutex)
{
   rt_mutex_delete((rt_mutex_t)mutex);
}

void os_usleep (uint32_t us)
{
   rt_thread_mdelay(us / 1000);
}

uint32_t os_get_current_time_us (void)
{
   return 1000 * rt_tick_get_millisecond();
}

os_tick_t os_tick_current (void)
{
   return rt_tick_get();
}

os_tick_t os_tick_from_us (uint32_t us)
{
   return rt_tick_from_millisecond(us / 1000);
}

void os_tick_sleep (os_tick_t tick)
{
   rt_thread_delay(tick);
}

os_sem_t * os_sem_create (size_t count)
{
   static unsigned short counter = 0;
   char tname[RT_NAME_MAX];

   rt_snprintf(tname, RT_NAME_MAX, "%s%d", OS_SEM_NAME, counter++);

   return (os_sem_t *)rt_sem_create(tname, count, RT_IPC_FLAG_FIFO);
}

bool os_sem_wait (os_sem_t * sem, uint32_t time)
{
   if (rt_sem_take((rt_sem_t)sem, TMO_TO_TICKS(time)) == RT_EOK) {
      /* Did not timeout */
      return false;
   }

   /* Timed out */
   return true;
}

void os_sem_signal (os_sem_t * sem)
{
   rt_sem_release((rt_sem_t)sem);
}

void os_sem_destroy (os_sem_t * sem)
{
   rt_sem_delete((rt_sem_t)sem);
}

os_event_t * os_event_create (void)
{
   static unsigned short counter = 0;
   char tname[RT_NAME_MAX];

   rt_snprintf(tname, RT_NAME_MAX, "%s%d", OS_EVENT_NAME, counter++);

   return (os_event_t *)rt_event_create(tname, RT_IPC_FLAG_FIFO);
}

bool os_event_wait (os_event_t * event, uint32_t mask, uint32_t * value, uint32_t time)
{
   rt_err_t ret = rt_event_recv((rt_event_t)event, mask, RT_EVENT_FLAG_OR, TMO_TO_TICKS(time), value);

   *value &= mask;

   return (ret != RT_EOK);
}

void os_event_set (os_event_t * event, uint32_t value)
{
   rt_event_send((rt_event_t)event, value);
}

void os_event_clr (os_event_t * event, uint32_t value)
{
   rt_event_recv((rt_event_t)event, value, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 0, NULL);
}

void os_event_destroy (os_event_t * event)
{
   rt_event_delete((rt_event_t)event);
}

os_mbox_t * os_mbox_create (size_t size)
{
   static unsigned short counter = 0;
   char tname[RT_NAME_MAX];

   rt_snprintf(tname, RT_NAME_MAX, "%s%d", OS_MBOX_NAME, counter++);

   return (os_mbox_t *)rt_mb_create(tname, size, RT_IPC_FLAG_FIFO);
}

bool os_mbox_fetch (os_mbox_t * mbox, void ** msg, uint32_t time)
{
   rt_err_t ret = rt_mb_recv((rt_mailbox_t)mbox, (rt_ubase_t *)msg, TMO_TO_TICKS(time));

   return (ret != RT_EOK);
}

bool os_mbox_post (os_mbox_t * mbox, void * msg, uint32_t time)
{
   rt_err_t ret = rt_mb_send_wait((rt_mailbox_t)mbox, (rt_ubase_t)msg, TMO_TO_TICKS(time));

   return (ret != RT_EOK);
}

void os_mbox_destroy (os_mbox_t * mbox)
{
   rt_mb_delete((rt_mailbox_t)mbox);
}

static void os_timer_callback(void *arg) {
   os_timer_t *timer = (os_timer_t *)arg;

   if (timer->fn)
      timer->fn(timer, timer->arg);
}

os_timer_t * os_timer_create (
   uint32_t us,
   void (*fn) (os_timer_t *, void * arg),
   void * arg,
   bool oneshot)
{
   static unsigned short counter = 0;
   char tname[RT_NAME_MAX];
   rt_uint8_t flag = 0;

   os_timer_t * timer = malloc (sizeof (*timer));
   CC_ASSERT (timer != NULL);

   timer->fn  = fn;
   timer->arg = arg;
   timer->us  = us;

   rt_snprintf(tname, RT_NAME_MAX, "%s%d", OS_TIMER_NAME, counter++);
   flag = RT_TIMER_FLAG_SOFT_TIMER;
   flag |= (oneshot ? RT_TIMER_FLAG_ONE_SHOT : RT_TIMER_FLAG_PERIODIC);
   timer->handle = rt_timer_create(tname, os_timer_callback, timer, rt_tick_from_millisecond(us / 1000), flag);
   CC_ASSERT (timer->handle != NULL);

   return timer;
}

void os_timer_set (os_timer_t * timer, uint32_t us)
{
   rt_tick_t tick = rt_tick_from_millisecond(us / 1000);
   timer->us = us;

   rt_timer_control(timer->handle, RT_TIMER_CTRL_SET_TIME, &tick);
}

void os_timer_start (os_timer_t * timer)
{
   rt_timer_start(timer->handle);
}

void os_timer_stop (os_timer_t * timer)
{
   rt_timer_stop(timer->handle);
}

void os_timer_destroy (os_timer_t * timer)
{
   rt_timer_delete(timer->handle);
   free(timer);
}
