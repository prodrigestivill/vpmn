/***************************************************************************
 *            tunsrvthread.c
 *
 *  Tue Feb 12 09:51:17 2008
 *  Copyright  2008  Pau Rodriguez-Estivill
 *  <prodrigestivill@gmail.com>
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tunsrvthread.h"
#include "protocol.h"
#include "debug.h"

void
tunsrvthread (struct tunsrvthread_t *me)
{
  pthread_mutex_lock (&me->cond_mutex);
  while (1)
    {
      pthread_cond_wait (&me->cond, &me->cond_mutex);
      protocol_sendframe (me->buffer, me->buffer_len);
      pthread_mutex_unlock (&me->thread_mutex);
    }
  pthread_mutex_unlock (&me->cond_mutex);
}

int
tunsrvthread_create (struct tunsrvthread_t *new)
{
  pthread_mutex_init (&new->thread_mutex, NULL);
  pthread_mutex_init (&new->cond_mutex, NULL);
  pthread_cond_init (&new->cond, NULL);
  return pthread_create (&new->thread, NULL, (void *) &tunsrvthread, new);
}
