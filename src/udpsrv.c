/***************************************************************************
 *            udpsrv.c
 *
 *  Tue Feb  5 11:22:05 2008
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
#include <stdlib.h>
#include <strings.h>
#include <resolv.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "udpsrvdtls.h"
#include "udpsrvsession.h"
#include "protocol.h"
#include "debug.h"
#include "config.h"
#include "srv.h"

int udpsrv_fd = -1;
pthread_cond_t udpsrv_waitcond;
pthread_mutex_t udpsrv_waitmutex;

struct udpsrv_thread_t
{
  pthread_t thread;
  pthread_mutex_t thread_mutex;
  pthread_cond_t cond;
  pthread_mutex_t cond_mutex;
  char buffer[UDPBUFFERSIZE];
  int buffer_len;
  struct sockaddr_in addr;
  socklen_t addr_len;
};

int
udpsrv_init ()
{
  struct sockaddr_in bind_addr;
  bzero (&bind_addr, sizeof (bind_addr));
  udpsrv_fd = socket (PF_INET, SOCK_DGRAM, 0);
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  bind_addr.sin_port = htons (port_udp);
  if (bind (udpsrv_fd, (struct sockaddr *) &bind_addr, sizeof (bind_addr)) !=
      0)
    {
      log_error ("Bind error\n");
      return -1;
    }
  return 0;
}

void
udpsrv_thread (struct udpsrv_thread_t *me)
{
  char tunbuffer[TUNBUFFERSIZE];
  int tunbuffer_len;
  struct sockaddr_in *addr;
  struct udpsrvsession_t *udpsession;
  pthread_mutex_lock (&me->cond_mutex);
  while (1)
    {
      pthread_cond_wait (&me->cond, &me->cond_mutex);
      addr = malloc (sizeof (struct sockaddr_in));
      memcpy (addr, &me->addr, me->addr_len);
      udpsession = udpsrvsession_search (addr);
      tunbuffer_len =
	udpsrvdtls_read (me->buffer, me->buffer_len, tunbuffer, TUNBUFFERSIZE,
			 udpsession);
      if (tunbuffer_len > 0)
	protocol_recvpacket (tunbuffer, tunbuffer_len, udpsession->peer);
      pthread_mutex_unlock (&me->thread_mutex);
      //Notify main loop about finished job
      pthread_mutex_lock (&udpsrv_waitmutex);
      pthread_cond_signal (&udpsrv_waitcond);
      pthread_mutex_unlock (&udpsrv_waitmutex);
    }
  pthread_mutex_unlock (&me->cond_mutex);
}

int
udpsrv_threadcreate (struct udpsrv_thread_t *new)
{
  pthread_mutex_init (&new->thread_mutex, NULL);
  pthread_mutex_init (&new->cond_mutex, NULL);
  pthread_cond_init (&new->cond, NULL);
  return pthread_create (&new->thread, NULL, (void *) &udpsrv_thread, new);
}

void
udpsrv ()
{
  int rc, th;
  struct udpsrv_thread_t udpsrvthreads[num_udpsrvthreads];

  pthread_mutex_init (&udpsrv_waitmutex, NULL);
  pthread_cond_init (&udpsrv_waitcond, NULL);

  for (th = 0; th < num_udpsrvthreads; th++)
    {
      if ((rc = udpsrv_threadcreate (&(udpsrvthreads[th]))))
	{
	  log_error ("Thread %d creation failed: %d\n", th, rc);
	  break;
	}
    }

  while (1)
    {
      for (th = 0; th < num_udpsrvthreads; th++)
	{
	  if (pthread_mutex_trylock (&udpsrvthreads[th].thread_mutex) == 0)
	    {
	      pthread_mutex_lock (&udpsrvthreads[th].cond_mutex);
	      udpsrvthreads[th].addr_len = sizeof (udpsrvthreads[th].addr);
	      bzero (&udpsrvthreads[th].addr, udpsrvthreads[th].addr_len);
	      udpsrvthreads[th].buffer_len =
		recvfrom (udpsrv_fd, udpsrvthreads[th].buffer,
			  sizeof (udpsrvthreads[th].buffer), 0,
			  (struct sockaddr *) &udpsrvthreads[th].addr,
			  &udpsrvthreads[th].addr_len);
	      if ((udpsrvthreads[th].buffer_len < 1)
		  || (pthread_cond_signal (&udpsrvthreads[th].cond) != 0))
		{
		  pthread_mutex_unlock (&udpsrvthreads[th].thread_mutex);
		  log_error ("Error reading form socket.\n");
		}
	      pthread_mutex_unlock (&udpsrvthreads[th].cond_mutex);
	    }
	}
      if (th >= num_udpsrvthreads)
	{
	  pthread_mutex_lock (&udpsrv_waitmutex);
	  pthread_cond_wait (&udpsrv_waitcond, &udpsrv_waitmutex);
	  pthread_mutex_unlock (&udpsrv_waitmutex);
	}
    }
}
