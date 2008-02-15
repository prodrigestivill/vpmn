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
#include "router.h"
#include "debug.h"

void
tunsrvthread (struct tunsrvthread_t *me)
{
  struct in_addr src4, dst4;
  struct peer_t *dstpeer;
  pthread_mutex_lock (&me->cond_mutex);
  while (1)
    {
      dstpeer = NULL;
      pthread_cond_wait (&me->cond, &me->cond_mutex);
      //Check for IPv4
      if (((me->buffer[0] & 240) >> 4) == 4)
	{
	  src4.s_addr = (me->buffer[12]) | (me->buffer[13] << 8) |
	    (me->buffer[14] << 16) | (me->buffer[15] << 24);
	  log_debug ("IPv4: %s", inet_ntoa (src4));
	  dst4.s_addr = (me->buffer[16]) | (me->buffer[17] << 8) |
	    (me->buffer[18] << 16) | (me->buffer[19] << 24);
	  log_debug ("->%s\n", inet_ntoa (dst4));
	  if (router_checksrc (&src4) == 0)
	    dstpeer = router_searchdst (&dst4);
	  else
	    log_error ("Invalid source.\n");
	}
      //Check for IPv6
      else if (((me->buffer[0] & 240) >> 4) == 6)
	{
	  log_error ("IPv6 not implemented.\n");
	  //ROUTING
	}
      else
	log_error ("Unknow protocol not implemented.\n");

      //DTLS
      if (dstpeer != NULL)
	{

	}
      else
	log_error ("Packet lost.\n");
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
