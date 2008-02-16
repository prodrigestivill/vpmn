/***************************************************************************
 *            router.c
 *
 *  Fri Feb 15 20:39:01 2008
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include "config.h"
#include "peer.h"

int
router_checksrc (struct in_addr *src)
{
  int n;
  if (src->s_addr == tunaddr_ip.addr.s_addr)
    return 0;
  if (tunaddr_networks == NULL || tunaddr_networks_len < 1)
    return -1;
  for (n = 0; n < tunaddr_networks_len; n++)
    {
      if ((src->s_addr & tunaddr_networks[n].netmask.s_addr) ==
	  tunaddr_networks[n].addr.s_addr)
	return 0;
    }
  return -2;
}
