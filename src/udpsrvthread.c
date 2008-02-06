/***************************************************************************
 *            udpsrvthread.c
 *
 *  Tue Feb  5 11:22:05 2008
 *  Copyright  2008  Pau Rodriguez-Estivill
 *  <prodrigestivill@gmail.com>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "udpsrvsession.h"
#include "udpsrvthread.h"
#include "debug.h"

void
udpsrvthread (struct udpsrvthread_t *me)
{
  char *s_addr;
  int s_port = 0;
  struct udpsrvsession_t *udpsession;
  log_debug ("Thread locking...\n");
  pthread_mutex_lock (&(me->cond_mutex));
  while (1)
    {
      log_debug ("Thread waiting...\n");
      pthread_cond_wait (&(me->cond), &(me->cond_mutex));
      //s_addr = inet_ntoa (me->addr.sin_addr);
      //s_port = ntohs (me->addr.sin_port);
      //udpsession = udpsrvsession_search (s_addr, s_port);
      log_debug ("Thread: \"%s\"\n", me->buffer);
      /*log_debug ("Thread: %s:%d \"%s\"\n", inet_ntoa (me->addr.sin_addr),
         ntohs (me->addr.sin_port), me->buffer); */
    }
  pthread_mutex_unlock (&(me->cond_mutex));
}

int
udpsrvthread_create (struct udpsrvthread_t *new)
{
  if (pthread_mutex_init (&(new->cond_mutex), NULL) != 0)
    {
      log_error ("pthread_mutex_init failed.\n");
      return -1;
    }
  if (pthread_cond_init (&(new->cond), NULL) != 0)
    {
      log_error ("pthread_cond_init failed.\n");
      return -1;
    }
  return pthread_create (&new->thread, NULL, (void *) &udpsrvthread, new);
}
