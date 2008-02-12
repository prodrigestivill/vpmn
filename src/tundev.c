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
 
#include "tundevthread.h"
#include <linux/if_tun.h>
#define DEFAULT_DEVICE 

char *device = "/dev/net/tun";
int num_tundevthreads = 4;

void
tundev ()
{
  int rc, th;
  int device_fd = -1;
  char *iface;
  char ifrname[IFNAMSIZ];

  struct ifreq ifr;
  struct tundevthread_t tundevthreads[num_tundevthreads];

  for (th = 0; th < num_tundevthreads; th++)
    {
      log_debug ("Creating thread %d...\n", th);
      if ((rc = udpsrvthread_create (&(tundevthreads[th]))))
	{
	  log_error ("Thread %d creation failed: %d\n", th, rc);
	  break;
	}
    }
    
    /*
     Begin TUN initzaitzation
     Copyright (C) 2001-2005 Ivo Timmermans,
         GPL       2001-2006 Guus Sliepen <guus@tinc-vpn.org>
    */
	iface = netname; //Linux
	//iface = rindex(device, '/') ? rindex(device, '/') + 1 : device;
	device_fd = open(device, O_RDWR | O_NONBLOCK);

	if(device_fd < 0)
      {
	    log_error ("Could not open %s: %s", device, strerror(errno));
        return;
      }

	memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN;
    
	if(iface)
		strncpy(ifr.ifr_name, iface, IFNAMSIZ);

	if(!ioctl(device_fd, TUNSETIFF, &ifr)) {
		strncpy(ifrname, ifr.ifr_name, IFNAMSIZ);
		iface = ifrname;
	} else if(!ioctl(device_fd, (('T' << 8) | 202), &ifr)) {
		strncpy(ifrname, ifr.ifr_name, IFNAMSIZ);
		iface = ifrname;
	} else
		//overwrite_mac = true;
		iface = rindex(device, '/') ? rindex(device, '/') + 1 : device;
	}

    //End TUN initzaitzation
}
