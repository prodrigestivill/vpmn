/***************************************************************************
 *            tundev.c
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

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "config.h"
#include "debug.h"
#include "tundevthread.h"

int tundev_fd = -1;

int
tundev_initdev (char *iface)
{
  int sd_tun = -1;
  int iface_len = strlen (iface);
  struct ifreq ifr;

  if ((sd_tun = open (TUNDEVICE, O_RDWR)) < 0)
    {
      log_error ("Could not open %s.\n", TUNDEVICE);
      return -1;
    }

  memset (&ifr, 0, sizeof (ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

  if (iface_len > 0)
    strncpy (ifr.ifr_name, iface, IFNAMSIZ);

  if (ioctl (sd_tun, TUNSETIFF, &ifr) < 0)
    {
      free (&ifr);
      close (sd_tun);
      log_error ("Could not create interface %s.\n", iface);
      return -1;
    }

  if (iface_len == 0)
    strncpy (iface, ifr.ifr_name, IFNAMSIZ);

  return sd_tun;
}

int
tundev_write (const void *buf, int count)
{
  if (tundev_fd >= 0)
    return write (tundev_fd, buf, count);
  return -1;
}

void
tundev ()
{
  int rc, th;
  char ifrname[IFNAMSIZ];
  struct tundevthread_t tundevthreads[num_tundevthreads];

  for (th = 0; th < num_tundevthreads; th++)
    {
      if ((rc = tundevthread_create (&(tundevthreads[th]))))
	{
	  log_error ("Thread %d creation failed: %d\n", th, rc);
	  break;
	}
    }

  strncpy (ifrname, tunname, IFNAMSIZ);
  tundev_fd = tundev_initdev (ifrname);
  if (tundev_fd < 0)
    {
      log_error ("Could not create interface %s.\n", iface);
      return;
    }
  while (1)
    {
      for (th = 0; th < num_tundevthreads; th++)
	{
	  if (pthread_mutex_trylock (&tundevthreads[th].thread_mutex) == 0)
	    {
	      pthread_mutex_lock (&tundevthreads[th].cond_mutex);
	      tundevthreads[th].buffer_len =
		read (tundev_fd, tundevthreads[th].buffer,
		      sizeof (tundevthreads[th].buffer));
	      if (tundevthreads[th].buffer_len > 0)
		{
		  pthread_cond_signal (&tundevthreads[th].cond);
		}
	      else
		{
		  pthread_mutex_unlock (&tundevthreads[th].thread_mutex);
		  log_error ("Error reading form interface %s\n", ifrname);
		}
	      pthread_mutex_unlock (&tundevthreads[th].cond_mutex);
	      break;
	    }
	}
    }
}
