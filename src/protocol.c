/***************************************************************************
 *            protocol.c
 *
 *  Tue Feb 19 15:13:39 2008
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

#include <stdlib.h>
#include "debug.h"
#include "config.h"
#include "protocol.h"
#include "peer.h"
#include "router.h"
#include "udpsrvdtls.h"
#include "tundev.h"

void
protocol_recvpacket (const char *tunbuffer, const int tunbuffer_len,
		     struct peer_t *peer)
{
  //IP packet
  if ((tunbuffer[0] & 0xF0) == 0x40 || (tunbuffer[0] & 0xF0) == 0x60)
    {
      tundev_write (tunbuffer, tunbuffer_len);
    }
  else				//Internal packets
    {
      switch (tunbuffer[0])
	{
	default:
	  log_error ("Unknow packet.\n");
	}
    }
}

void
protocol_sendframe (const char *buffer, const int buffer_len)
{
  struct in_addr src4, dst4;
  struct peer_t *dstpeer = NULL;
  //Check for IPv4
  if ((buffer[0] & 0xF0) == 0x40)
    {
      src4.s_addr = (buffer[12]) | (buffer[13] << 8) |
	(buffer[14] << 16) | (buffer[15] << 24);
      dst4.s_addr = (buffer[16]) | (buffer[17] << 8) |
	(buffer[18] << 16) | (buffer[19] << 24);
      if (router_checksrc (&src4) == 0)
	dstpeer = router_searchdst (&dst4);
      else
	log_error ("Invalid source.\n");
    }
  //Check for IPv6
  else if ((buffer[0] & 0xF0) == 0x60)
    {
      log_error ("IPv6 not implemented.\n");
      //ROUTING IPv6
    }
  else
    log_error ("Unknow protocol not implemented.\n");

  //CRYPTO
  if (dstpeer != NULL && dstpeer->udpsrvsession != NULL)
    udpsrvdtls_write (buffer, buffer_len, dstpeer->udpsrvsession);
  else
    log_error ("Packet lost.\n");
}

void
protocol_sendroutes (const struct peer_t *dstpeer)
{
  int len;
  struct protocol_route table;
  table.p = '\0';
  if (tunaddr_networks_len > MAX_PROTOCOL_ROUTES)
    len = MAX_PROTOCOL_ROUTES;
  else
    len = tunaddr_networks_len;
  memcpy (&table.routes, &tunaddr_networks, len * sizeof (struct in_network));
  if (dstpeer->udpsrvsession != NULL)
    udpsrvdtls_write ((char *) &table, sizeof (table),
		      dstpeer->udpsrvsession);
}
