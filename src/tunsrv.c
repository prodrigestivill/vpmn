/***************************************************************************
 *            tunsrv.c
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
#include "config.h"
#include "debug.h"
#include "tundev.h"
#include "tunsrvthread.h"

void
tunsrv ()
{
  int rc, th;
  struct tunsrvthread_t tunsrvthreads[num_tunsrvthreads];

  for (th = 0; th < num_tunsrvthreads; th++)
    {
      if ((rc = tunsrvthread_create (&(tunsrvthreads[th]))))
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
    }
}
