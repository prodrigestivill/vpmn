/***************************************************************************
 *            config.c
 *
 *  Thu Feb 14 18:40:38 2008
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
#include "config.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void
config_load ()
{
  //TUNDEV
  tunname = "vpmn0";

  char *tunaddr_ip_str = "10.0.0.5";
  char *tunaddr_nm_str = "255.255.255.0";
  if (inet_aton (tunaddr_ip_str, &tunaddr_ip.addr) == 0
      || inet_aton (tunaddr_nm_str, &tunaddr_ip.netmask) == 0)
    log_error ("Loading IP configurations.");

  char *tunaddr_net0_ip_str = "10.1.0.5";
  char *tunaddr_net0_nm_str = "255.255.255.0";
  tunaddr_networks = calloc (1, sizeof (struct in_network));
  inet_aton (tunaddr_net0_ip_str, &tunaddr_networks[0].addr);
  inet_aton (tunaddr_net0_nm_str, &tunaddr_networks[0].netmask);
  tunaddr_networks[0].addr.s_addr =
    tunaddr_networks[0].addr.s_addr & tunaddr_networks[0].netmask.s_addr;
  tunaddr_networks_len = 1;

  num_tunsrvthreads = 10;
  tundevmtu = 1450;

  //UDPSRV
  num_udpsrvthreads = 4;
  port_udp = 1090;
}
