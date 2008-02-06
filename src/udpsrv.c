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

#include <sys/socket.h>
#include <resolv.h>
#include <pthread.h>
#include <stdlib.h>
#include <strings.h>

#include <netinet/in.h>
#include <arpa/inet.h>


#include "udpsrvthread.h"
#include "debug.h"

int num_threads = 4;
int udp_port = 1090;
int buffer_size = 1024;

void
udpsrv ()
{
  int rc, th;
  struct udpsrvthread_t udpsrvthreads[num_threads];

  for (th = 0; th < num_threads; th++)
    {
      log_debug ("Creating thread %d...\n", th);
      if ((rc = udpsrvthread_create(&(udpsrvthreads[th]))))
		{
	  log_error ("Thread %d creation failed: %d\n", th, rc);
	  break;
		}
    }
  sleep (1);
  log_debug ("Starting...\n");
  int sd_udp;
  struct sockaddr_in bind_addr;
  sd_udp = socket (PF_INET, SOCK_DGRAM, 0);
  bzero (&bind_addr, sizeof (bind_addr));
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_port = htons (udp_port);
  bind_addr.sin_addr.s_addr = INADDR_ANY;
  if (bind (sd_udp, (struct sockaddr *) &bind_addr, sizeof (bind_addr)) != 0)
    log_error ("Bind error\n");
  log_debug ("Listening...\n");
  while (1)
    {
      for (th = 0; th < num_threads; th++)
	{
	  log_debug ("Try thread %d...", th);
	  if (pthread_mutex_trylock (&(udpsrvthreads[th].cond_mutex)) == 0)
	    {
	      log_debug (" selected.\n");
	      udpsrvthreads[th].addr_len = sizeof (udpsrvthreads[th].addr);
	      bzero (&udpsrvthreads[th].addr, udpsrvthreads[th].addr_len);
	      udpsrvthreads[th].buffer = malloc (buffer_size * sizeof (char));
	      udpsrvthreads[th].buffer_len = recvfrom (sd_udp, udpsrvthreads[th].buffer, sizeof (udpsrvthreads[th].buffer), 0, &(udpsrvthreads[th].addr), &(udpsrvthreads[th].addr_len));
	      log_debug ("Main  : %s\n", //:%d \"%s\"\n", "",0,
//			 inet_ntoa (udpsrvthreads[th].addr.sin_addr), 0,
//			 ntohs (udpsrvthreads[th].addr.sin_port),
			 udpsrvthreads[th].buffer);
	      if (pthread_cond_signal (&(udpsrvthreads[th].cond)) == 0){
pthread_mutex_unlock (&(udpsrvthreads[th].cond_mutex));
	      break;
			}else{
pthread_mutex_unlock (&(udpsrvthreads[th].cond_mutex));
				log_error("Can't wake up the thread.\n");
}
	      
	    }
	  else
	    log_debug (" busy.\n");

	}
      if (th >= num_threads)
	log_debug ("All threads busy, trying again.");

sleep(1);
    }
}

int
main ()
{
  udpsrv ();
}
