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
#include <fcntl.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "config.h"
#include "debug.h"

#define TUNDEVICE "/dev/net/tun"
int tundev_fd = -1;

int
tundev_initdev ()
{
  int sd_sock = -1, tunname_len = strlen (tunname);
  struct sockaddr_in *tunaddr;
  struct ifreq ifr;


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
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  if (tunname_len > 0)
    strncpy (ifr.ifr_name, tunname, IFNAMSIZ);

  if (ioctl (tundev_fd, TUNSETIFF, &ifr) < 0)
    {
      close (tundev_fd);
      return -1;
    }
  if (tunname_len == 0)
    strncpy (tunname, ifr.ifr_name, IFNAMSIZ);

  if ((sd_sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      log_error ("Cannot open socket and configure the interface %s.\n",
		 tunname);
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
      tunaddr = (struct sockaddr_in *) &ifr.ifr_addr;
      tunaddr->sin_family = AF_INET;
      tunaddr->sin_addr.s_addr = tunaddr_ip.addr.s_addr;
      if (ioctl (sd_sock, SIOCSIFADDR, &ifr) < 0)
	{
	  log_error ("Could not configure inet addr %s in the interface.\n",
		     inet_ntoa (tunaddr_ip.addr));
	}
      tunaddr = (struct sockaddr_in *) &ifr.ifr_netmask;
      tunaddr->sin_family = AF_INET;
      tunaddr->sin_addr.s_addr = tunaddr_ip.netmask.s_addr;
      if (ioctl (sd_sock, SIOCSIFNETMASK, &ifr) < 0)
	{
	  log_error ("Could not configure netmask %s in the interface.\n",
		     inet_ntoa (tunaddr_ip.netmask));
	}
      tunaddr = (struct sockaddr_in *) &ifr.ifr_dstaddr;
      tunaddr->sin_family = AF_INET;
      tunaddr->sin_addr.s_addr = tunaddr_ip.addr.s_addr & tunaddr_ip.netmask.s_addr;
      if (ioctl (sd_sock, SIOCSIFDSTADDR, &ifr) < 0)
	{
	  log_error ("Could not configure net addr %s in the interface.\n",
		     inet_ntoa (tunaddr->sin_addr));
	}
      ioctl (sd_sock, SIOCGIFFLAGS, &ifr);
      ifr.ifr_flags |= IFF_UP;
      if (ioctl (sd_sock, SIOCSIFFLAGS, &ifr) < 0)
	{
	  log_error ("Could not configure flags of the interface.\n");
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

int
tundev_read (void *buf, int count)
{
  if (tundev_fd >= 0)
    return read (tundev_fd, buf, count);
  return -1;
}
