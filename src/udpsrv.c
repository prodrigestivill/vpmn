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

#include <sys/socket.h>
#include <resolv.h>
#include <pthread.h>
#include <stdlib.h>
#include <strings.h>

#include <netinet/in.h>
#include <arpa/inet.h>


#include "udpsrvthread.h"
#include "debug.h"

int num_udpsrvthreads = 4;
int port_udp = 1090;

void
udpsrv ()
{
  int rc, th, sd_udp;
  struct sockaddr_in bind_addr;
  struct udpsrvthread_t udpsrvthreads[num_udpsrvthreads];

  for (th = 0; th < num_udpsrvthreads; th++)
    {
      if ((rc = udpsrvthread_create (&(udpsrvthreads[th]))))
	{
	  log_error ("Thread %d creation failed: %d\n", th, rc);
	  break;
	}
    }
  bzero (&bind_addr, sizeof (bind_addr));
  sd_udp = socket (PF_INET, SOCK_DGRAM, 0);
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  bind_addr.sin_port = htons (port_udp);
  if (bind (sd_udp, (struct sockaddr *) &bind_addr, sizeof (bind_addr)) != 0)
    log_error ("Bind error\n");
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
		recvfrom (sd_udp, udpsrvthreads[th].buffer,
			  sizeof (udpsrvthreads[th].buffer), 0,
			  (struct sockaddr *) &udpsrvthreads[th].addr,
			  &udpsrvthreads[th].addr_len);
	      if (pthread_cond_signal (&udpsrvthreads[th].cond) == 0)
		{
		  pthread_mutex_unlock (&udpsrvthreads[th].cond_mutex);
		  break;
		}
	      else
		{
		  pthread_mutex_unlock (&udpsrvthreads[th].cond_mutex);
		  log_error ("Can't wake up the thread.\n");
		}

	    }
	}
      /*if (th >= num_udpsrvthreads)
         sleep */
    }
}
