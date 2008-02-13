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

#include "debug.h"
#include "tundevthread.h"
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#define DEFAULT_DEVICE

char *tundevice = "/dev/net/tun";
char *tunname = "vpmn0";
int num_tundevthreads = 10;
int tunmtu = 1500;

void
tundev ()
{
  int rc, th;
  int sd_tun = -1;
  char *iface;
  char ifrname[IFNAMSIZ];

  struct ifreq ifr;
  struct tundevthread_t tundevthreads[num_tundevthreads];

  for (th = 0; th < num_tundevthreads; th++)
    {
      log_debug ("Creating thread %d...\n", th);
      if ((rc = tundevthread_create (&(tundevthreads[th]))))
	{
	  log_error ("Thread %d creation failed: %d\n", th, rc);
	  break;
	}
    }

  // Begin TUN initzaitzation
  //iface = rindex(tundevice, '/') ? rindex(tundevice, '/') + 1 : tundevice;
  iface = tunname;
  sd_tun = open (tundevice, O_RDWR);	// | O_NONBLOCK);

  if (sd_tun < 0)
    {
      log_error ("Could not open %s.\n", tundevice);
      return;
    }

  memset (&ifr, 0, sizeof (ifr));
  ifr.ifr_flags = IFF_TUN;

  if (iface)
    strncpy (ifr.ifr_name, iface, IFNAMSIZ);

  if (!ioctl (sd_tun, TUNSETIFF, &ifr))
    {
      strncpy (ifrname, ifr.ifr_name, IFNAMSIZ);
      iface = ifrname;
    }
  else if (!ioctl (sd_tun, (('T' << 8) | 202), &ifr))
    {
      strncpy (ifrname, ifr.ifr_name, IFNAMSIZ);
      iface = ifrname;
    }
  else
    {
      //overwrite_mac = true;
      iface =
	rindex (tundevice, '/') ? rindex (tundevice, '/') + 1 : tundevice;
    }

  //End TUN initzaitzation
  while (1)
    {
      for (th = 0; th < num_tundevthreads; th++)
	{
	  if (pthread_mutex_trylock (&tundevthreads[th].thread_mutex) == 0)
	    {
	      pthread_mutex_lock (&tundevthreads[th].cond_mutex);
	      tundevthreads[th].buffer_len =
		read (sd_tun, tundevthreads[th].buffer,
		      sizeof (tundevthreads[th].buffer));
	      if (tundevthreads[th].buffer_len > 0)
		{
		  pthread_cond_signal (&tundevthreads[th].cond);
		}
	      else
		{
		  pthread_mutex_unlock (&tundevthreads[th].thread_mutex);
		  log_error ("Error reading form interface %s\n", iface);
		}
	      pthread_mutex_unlock (&tundevthreads[th].cond_mutex);
	      break;
	    }
	}
    }
}
