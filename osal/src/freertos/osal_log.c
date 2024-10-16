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

#include "osal_log.h"

#include <stdarg.h>
#include <stdio.h>

void os_log (uint8_t type, const char * fmt, ...)
{
   va_list list;

   switch (LOG_LEVEL_GET (type))
   {
   case LOG_LEVEL_DEBUG:
      rt_kprintf ("%10ld [DEBUG] ", rt_tick_get());
      break;
   case LOG_LEVEL_INFO:
      rt_kprintf ("%10ld [INFO ] ", rt_tick_get());
      break;
   case LOG_LEVEL_WARNING:
      rt_kprintf ("%10ld [WARN ] ", rt_tick_get());
      break;
   case LOG_LEVEL_ERROR:
      rt_kprintf ("%10ld [ERROR] ", rt_tick_get());
      break;
   case LOG_LEVEL_FATAL:
      rt_kprintf ("%10ld [FATAL] ", rt_tick_get());
      break;
   default:
      break;
   }

   va_start (list, fmt);
   vrt_kprintf (fmt, list);
   va_end (list);
}
