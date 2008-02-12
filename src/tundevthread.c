/***************************************************************************
 *            tundevthread.c
 *
 *  Tue Feb 12 09:51:17 2008
 *  Copyright  2008  plue
 *  <plue@<host>>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */
 
#include <pthread.h>
#include "tundevthread.h"
#include "debug.h"

void
tundevthread (struct tundevthread_t *me)
{
  log_debug ("Thread locking...\n");
  pthread_mutex_lock (&me->cond_mutex);
  while (1)
    {
      log_debug ("Thread waiting...\n");
      pthread_cond_wait (&me->cond, &me->cond_mutex);
      log_debug ("%s\n", &me->buffer);

      pthread_mutex_unlock (&me->thread_mutex);
    }
  pthread_mutex_unlock (&me->cond_mutex);
}

int
tundevthread_create (struct tundevthread_t *new)
{
  pthread_mutex_init (&new->thread_mutex, NULL);
  pthread_mutex_init (&new->cond_mutex, NULL);
  pthread_cond_init (&new->cond, NULL);
  return pthread_create (&new->thread, NULL, (void *) &tundevthread, new);
}
