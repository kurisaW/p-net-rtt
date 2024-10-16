/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2021 rt-labs AB, Sweden.
 *
 * This software is licensed under the terms of the BSD 3-clause
 * license. See the file LICENSE distributed with this software for
 * full license information.
 ********************************************************************/

#include "osal.h"

#include <stdio.h>
#include <stdlib.h>

static int mutex_handle_counter = 0;
static int sem_handle_counter = 0;
static int event_handle_counter = 0;
static int mq_handle_counter = 0;

#define TMO_TO_TICKS(ms) \
   ((ms == OS_WAIT_FOREVER) ? RT_TICK_MAX : (ms) / RT_TICK_PER_SECOND)

void * os_malloc (size_t size)
{
   return rt_malloc (size);
}

void os_free (void * ptr)
{
   rt_free (ptr);
}

os_thread_t * os_thread_create (
   const char * name,
   uint32_t priority,
   size_t stacksize,
   void (*entry) (void * arg),
   void * arg)
{
   rt_thread_t handle = rt_thread_create(name, entry, arg, stacksize, priority, 10);
   if(handle != RT_NULL)
   {
      rt_thread_startup(handle);
      return (os_thread_t *)handle;
   }
   else
   {
      return RT_NULL;
   }
}

os_mutex_t * os_mutex_create (void)
{
   char mutex_name[RT_NAME_MAX];

   rt_snprintf(mutex_name, sizeof(mutex_name), "os_mutex%d", mutex_handle_counter);
   mutex_handle_counter++;

   rt_mutex_t handle = rt_mutex_create(mutex_name, RT_IPC_FLAG_PRIO);
   RT_ASSERT (handle != RT_NULL);
   return (os_mutex_t *)handle;
}

void os_mutex_lock (os_mutex_t * mutex)
{
   rt_mutex_take((rt_mutex_t) mutex, RT_WAITING_FOREVER);
}

void os_mutex_unlock (os_mutex_t * mutex)
{
   rt_mutex_release((rt_mutex_t) mutex);
}

void os_mutex_destroy (os_mutex_t * mutex)
{
   rt_mutex_delete((rt_mutex_t) mutex);
}

void os_usleep (uint32_t us)
{
   rt_kprintf("Please implement us delay based on a hwtimer!\n");
}

uint32_t os_get_current_time_us (void)
{
   return 1000 * (rt_tick_get() / RT_TICK_PER_SECOND);
}

os_sem_t * os_sem_create (size_t count)
{
   char sem_name[RT_NAME_MAX];

   rt_snprintf(sem_name, sizeof(sem_name), "os_sem%d", sem_handle_counter);
   sem_handle_counter++;

   rt_sem_t handle = rt_sem_create(sem_name, count, RT_IPC_FLAG_PRIO);
   RT_ASSERT (handle != RT_NULL);
   return (os_sem_t *)handle;
}

bool os_sem_wait (os_sem_t * sem, uint32_t time)
{
   if (rt_sem_take ((rt_sem_t)sem, TMO_TO_TICKS (time)) == RT_EOK)
   {
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
   char event_name[RT_NAME_MAX];

   rt_snprintf(event_name, sizeof(event_name), "os_event%d", event_handle_counter);
   event_handle_counter++;

   rt_event_t handle = rt_event_create(event_name, RT_IPC_FLAG_PRIO);
   RT_ASSERT (handle != RT_NULL);
   return (os_event_t *)handle;
}

bool os_event_wait (os_event_t * event, uint32_t mask, uint32_t * value, uint32_t time)
{
   rt_uint32_t recv_val = RT_NULL;
   rt_uint32_t result = rt_event_recv((rt_event_t)event, mask, RT_EVENT_FLAG_CLEAR, TMO_TO_TICKS (time), &recv_val);
   
   if (result == RT_EOK)
   {
      return RT_TRUE;
   }

   return RT_FALSE;
}

void os_event_set (os_event_t * event, uint32_t value)
{
   rt_event_send((rt_event_t)event, value);
}

void os_event_clr (os_event_t * event, uint32_t value)
{
   return;
}

void os_event_destroy (os_event_t * event)
{
   rt_event_delete((rt_event_t)event);
}

os_mbox_t * os_mbox_create (size_t size)
{
   QueueHandle_t handle = xQueueCreate (size, sizeof (void *));
   CC_ASSERT (handle);
   return (os_mbox_t *)handle;

   char mq_name[RT_NAME_MAX];

   rt_snprintf(mq_name, sizeof(mq_name), "os_mq%d", mq_handle_counter);
   mq_handle_counter++;

   rt_mq_t handle = rt_mq_create(mq_name, size, sizeof(size), RT_IPC_FLAG_FIFO);
   RT_ASSERT (handle != RT_NULL);
   return (os_mbox_t *)handle;
}

bool os_mbox_fetch (os_mbox_t * mbox, void ** msg, uint32_t time)
{
   if (rt_mq_recv((rt_mq_t)mbox, msg, sizeof(void *), TMO_TO_TICKS (time)) > RT_NULL)
   {
      return RT_TRUE;
   }

   return RT_FALSE;
}

bool os_mbox_post (os_mbox_t * mbox, void * msg, uint32_t time)
{
   rt_err_t success;

   success = rt_mq_send_wait((rt_mq_t)mbox, &msg, sizeof(msg), TMO_TO_TICKS (time));

   return success != RT_EOK;
}

void os_mbox_destroy (os_mbox_t * mbox)
{
   vQueueDelete ((QueueHandle_t)mbox);
}

static void os_timer_callback (void *parameter)
{
   os_timer_t * timer = (os_timer_t *)parameter;

   if (timer->fn)
      timer->fn (timer, timer->arg);
}

os_timer_t * os_timer_create (
   uint32_t us,
   void (*fn) (os_timer_t *, void * arg),
   void * arg,
   bool oneshot)
{
   os_timer_t * timer;

   timer = rt_malloc (sizeof (*timer));
   CC_ASSERT (timer != NULL);

   timer->fn  = fn;
   timer->arg = arg;
   timer->us  = us;

   timer->handle = rt_timer_create("os_timer", fn, (us / RT_TICK_PER_SECOND / 1000), (oneshot ? RT_TIMER_FLAG_ONE_SHOT : RT_TIMER_FLAG_PERIODIC));
   CC_ASSERT (timer->handle != RT_NULL);
   return timer;
}

void os_timer_set (os_timer_t * timer, uint32_t us)
{
   timer->us = us;
}

void os_timer_start (os_timer_t * timer)
{
   /* Start timer by updating the period */
   rt_timer_start((rt_timer_t)timer);
}

void os_timer_stop (os_timer_t * timer)
{
   CC_ASSERT (xTimerStop (timer->handle, portMAX_DELAY) == pdPASS);
   rt_timer_stop((rt_timer_t)timer);
}

void os_timer_destroy (os_timer_t * timer)
{
   rt_timer_detach((rt_timer_t)timer);
   rt_free (timer);
}

void assert_report (bool exp, const char *file, const char *function, int line)
{
   if(!exp){
		rt_printf("assert! file:%s, fuction:%s, line:%05d\n", file, function, line);
      while(1){
      //   osThreadTerminate(NULL);
      }
   }
}
