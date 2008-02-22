/***************************************************************************
 *            tunsrv.c
 *
 *  VPMN  -  Virtual Private Mesh Network
 *  Copyright  2008  Pau Rodriguez-Estivill
 *  <prodrigestivill@gmail.com>
 ****************************************************************************/

/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "protocol.h"
#include "tundev.h"
#include "config.h"
#include "debug.h"

pthread_cond_t tunsrv_waitcond;
pthread_mutex_t tunsrv_waitmutex;

struct tunsrv_thread_t
{
  pthread_t thread;
  pthread_mutex_t thread_mutex;
  pthread_cond_t cond;
  pthread_mutex_t cond_mutex;
  char buffer[TUNBUFFERSIZE];
  int buffer_len;
};

void
tunsrv_thread (struct tunsrv_thread_t *me)
{
  pthread_mutex_lock (&me->cond_mutex);
  while (1)
    {
      pthread_cond_wait (&me->cond, &me->cond_mutex);
      protocol_sendframe (me->buffer, me->buffer_len);
      pthread_mutex_unlock (&me->thread_mutex);
      //Notify main loop about finished job
      pthread_mutex_lock (&tunsrv_waitmutex);
      pthread_cond_signal (&tunsrv_waitcond);
      pthread_mutex_unlock (&tunsrv_waitmutex);
    }
  pthread_mutex_unlock (&me->cond_mutex);
}

int
tunsrv_threadcreate (struct tunsrv_thread_t *new)
{
  pthread_mutex_init (&new->thread_mutex, NULL);
  pthread_mutex_init (&new->cond_mutex, NULL);
  pthread_cond_init (&new->cond, NULL);
  return pthread_create (&new->thread, NULL, (void *) &tunsrv_thread, new);
}

void
tunsrv ()
{
  int rc, th;
  struct tunsrv_thread_t tunsrvthreads[num_tunsrvthreads];

  pthread_mutex_init (&tunsrv_waitmutex, NULL);
  pthread_cond_init (&tunsrv_waitcond, NULL);


  for (th = 0; th < num_tunsrvthreads; th++)
    {
      if ((rc = tunsrv_threadcreate (&(tunsrvthreads[th]))))
	{
	  log_error ("Thread %d creation failed: %d\n", th, rc);
	  break;
	}
    }
  while (1)
    {
      for (th = 0; th < num_tunsrvthreads; th++)
	{
	  if (pthread_mutex_trylock (&tunsrvthreads[th].thread_mutex) == 0)
	    {
	      pthread_mutex_lock (&tunsrvthreads[th].cond_mutex);
	      tunsrvthreads[th].buffer_len =
		tundev_read (tunsrvthreads[th].buffer,
			     sizeof (tunsrvthreads[th].buffer));
	      if (tunsrvthreads[th].buffer_len > 0)
		{
		  pthread_cond_signal (&tunsrvthreads[th].cond);
		}
	      else
		{
		  pthread_mutex_unlock (&tunsrvthreads[th].thread_mutex);
		  log_error ("Error reading form interface.\n");
		}
	      pthread_mutex_unlock (&tunsrvthreads[th].cond_mutex);
	      break;
	    }
	}
      //Wait for free thread
      if (th >= num_tunsrvthreads)
	{
	  pthread_mutex_lock (&tunsrv_waitmutex);
	  pthread_cond_wait (&tunsrv_waitcond, &tunsrv_waitmutex);
	  pthread_mutex_unlock (&tunsrv_waitmutex);
	}
    }
}
