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
#include <sys/socket.h>
#include <arpa/inet.h>
#include "config.h"
#include "debug.h"
#include "tundevthread.h"

int tundev_fd = -1;

int
tundev_initdev ()
{
  int sd_sock, iface_len;
  struct sockaddr_in tunaddr_dst;
  struct ifreq ifr;
  char iface[IFNAMSIZ];
  strncpy (iface, tunname, IFNAMSIZ);
  iface_len = strlen (iface);

  if (tundev_fd != -1)
    {
      log_error ("Already initalitzed.\n");
      return -1;
    }
  if ((tundev_fd = open (TUNDEVICE, O_RDWR)) < 0)
    {
      log_error ("Could not open %s.\n", TUNDEVICE);
      return -1;
    }

  memset (&ifr, 0, sizeof (ifr));
  ifr.ifr_flags = IFF_TUN;	// | IFF_NO_PI;
  if (iface_len > 0)
    strncpy (ifr.ifr_name, iface, IFNAMSIZ);

  if (ioctl (tundev_fd, TUNSETIFF, &ifr) < 0)
    {
      close (tundev_fd);
      return -1;
    }
  if (iface_len == 0)
    strncpy (iface, ifr.ifr_name, IFNAMSIZ);

  if ((sd_sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      log_error ("Cannot open socket and configure the interface %s.\n",
		 iface);
    }
  else
    {
      ifr.ifr_flags = 0;
      ifr.ifr_mtu = tundevmtu;
      if (ioctl (sd_sock, SIOCSIFMTU, &ifr) < 0)
	{
	  log_error ("Could not configure mtu %d in the interface.\n",
		     ifr.ifr_mtu);
	}
      memcpy (&ifr.ifr_addr, &tunaddr_ip, sizeof (struct sockaddr));
      if (ioctl (sd_sock, SIOCSIFADDR, &ifr) < 0)
	{
	  log_error ("Could not configure inet addr %s in the interface.\n",
		     inet_ntoa (tunaddr_ip.sin_addr));
	}
      memcpy (&ifr.ifr_netmask, &tunaddr_nm, sizeof (struct sockaddr));
      if (ioctl (sd_sock, SIOCSIFNETMASK, &ifr) < 0)
	{
	  log_error ("Could not configure netmask %s in the interface.\n",
		     inet_ntoa (tunaddr_nm.sin_addr));
	}
      tunaddr_dst.sin_family = AF_INET;
      tunaddr_dst.sin_addr.s_addr =
	tunaddr_ip.sin_addr.s_addr & tunaddr_nm.sin_addr.s_addr;
      memcpy (&ifr.ifr_dstaddr, &tunaddr_dst, sizeof (struct sockaddr));
      if (ioctl (sd_sock, SIOCSIFDSTADDR, &ifr) < 0)
	{
	  log_error ("Could not configure net addr %s in the interface.\n",
		     inet_ntoa (tunaddr_dst.sin_addr));
	}
      ioctl (sd_sock, SIOCGIFFLAGS, &ifr);
      ifr.ifr_flags |= IFF_UP;
      if (ioctl (sd_sock, SIOCSIFFLAGS, &ifr) < 0)
	{
	  log_error ("Could not configure flags of the interface.\n", iface);
	}
      close (sd_sock);
    }

  return tundev_fd;
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
		read (tundev_fd, tundevthreads[th].buffer,
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
