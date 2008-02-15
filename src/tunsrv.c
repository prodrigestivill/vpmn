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
#include "tundevthread.h"

void
tunsrv ()
{
  int rc, th;
  struct tundevthread_t tundevthreads[num_tundevthreads];

  for (th = 0; th < num_tundevthreads; th++)
    {
      if ((rc = tundevthread_create (&(tundevthreads[th]))))
	{
	  log_error ("Thread %d creation failed: %d\n", th, rc);
	  break;
	}
    }
  while (1)
    {
      for (th = 0; th < num_tundevthreads; th++)
	{
	  if (pthread_mutex_trylock (&tundevthreads[th].thread_mutex) == 0)
	    {
	      pthread_mutex_lock (&tundevthreads[th].cond_mutex);
	      tundevthreads[th].buffer_len =
		tundev_read (tundevthreads[th].buffer,
			     sizeof (tundevthreads[th].buffer));
	      if (tundevthreads[th].buffer_len > 0)
		{
		  pthread_cond_signal (&tundevthreads[th].cond);
		}
	      else
		{
		  pthread_mutex_unlock (&tundevthreads[th].thread_mutex);
		  log_error ("Error reading form interface.\n");
		}
	      pthread_mutex_unlock (&tundevthreads[th].cond_mutex);
	      break;
	    }
	}
    }
}
