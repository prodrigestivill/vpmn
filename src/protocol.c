/***************************************************************************
 *            protocol.c
 *
 *  VPMN  -  Virtual Private Mesh Network
 *  Copyright  2008  Pau Rodriguez-Estivill
 *  <prodrigestivill@gmail.com>
 ****************************************************************************/

/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include "debug.h"
#include "config.h"
#include "protocol.h"
#include "peer.h"
#include "router.h"
#include "udpsrvdtls.h"
#include "tundev.h"

char *protocol_v1id;
int protocol_v1id_len;
char *protocol_v1ka;
int protocol_v1ka_len;
int protocol_v1ka_pos;

void
protocol_recvpacket (const char *buffer, const int buffer_len,
		     void *session, const int sessiontype)
{
  struct in_addr src4, dst4;
  struct peer_t *peer = NULL;
  if (buffer_len < 2)
    return;
  if (sessiontype == UDPSRVSESSION && session != NULL)
    peer = ((struct udpsrvsession_t *) session)->peer;
  //IP packet
  if ((buffer[0] & 0xF0) == 0x40 || (buffer[0] & 0xF0) == 0x60)
    {
      if (peer == NULL || buffer_len < 20)
	return;
      //Check if it is an interested packet.
      dst4.s_addr = (buffer[16]) | (buffer[17] << 8) |
	(buffer[18] << 16) | (buffer[19] << 24);
      if (router_checksrc (&dst4, &tun_selfpeer) != 0)
	return;
      //Check permisions
      src4.s_addr = (buffer[12]) | (buffer[13] << 8) |
	(buffer[14] << 16) | (buffer[15] << 24);
      if (router_checksrc (&src4, peer) != 0)
	return;
      tundev_write (buffer, buffer_len);
    }
  else				//Internal packets
    {
      switch (buffer[0])
	{
	case PROTOCOL1_ID:

	  break;
	case PROTOCOL1_KA:

	  break;
	default:
	  log_error ("Unknow packet.\n");
	}
    }
}

int
protocol_sendraw (const char *buffer, const int buffer_len,
		  const struct peer_t *dstpeer)
{
  if (dstpeer->udpsrvsession != NULL)
    {
      udpsrvdtls_write (buffer, buffer_len, dstpeer->udpsrvsession);
      return UDPSRVSESSION;
    }
  return NOSESSION;
}

int
protocol_sendframe (const char *buffer, const int buffer_len)
{
  struct in_addr src4, dst4;
  struct peer_t *dstpeer = NULL;
  if (buffer_len < 20)
    return NOSESSION;
  //Check for IPv4
  if ((buffer[0] & 0xF0) == 0x40)
    {
      src4.s_addr = (buffer[12]) | (buffer[13] << 8) |
	(buffer[14] << 16) | (buffer[15] << 24);
      dst4.s_addr = (buffer[16]) | (buffer[17] << 8) |
	(buffer[18] << 16) | (buffer[19] << 24);
      if (router_checksrc (&src4, &tun_selfpeer) == 0)
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
  if (buffer_len < 1)
    return NOSESSION;
  return protocol_sendraw (buffer, buffer_len, dstpeer);
}

int
protocol_sendpacket (const struct peer_t *dstpeer, const int type)
{
  char *packet;
  int packet_len = -1;
  switch (type)
    {
    case PROTOCOL1_ID:
      packet = protocol_v1id;
      packet_len = protocol_v1id_len;
      break;
    case PROTOCOL1_KA:
      packet = protocol_v1ka;
      packet_len = protocol_v1ka_len;
      break;
    }
  if (packet_len < 1)
    return NOSESSION;
  return protocol_sendraw (packet, packet_len, dstpeer);
}

void
protocol_slidekeepalive ()
{
/*protocol_v1ka;
  protocol_v1ka_len;
  protocol_v1ka_pos;*/
}

void
protocol_init ()
{
/*-TODO
  protocol_v1id;
  protocol_v1id_len;*/
  protocol_v1ka_pos = 0;
  protocol_slidekeepalive ();
}

void
protocol_maintainerthread ()
{
  struct peer_t *dstpeer;
  protocol_init ();
/*-TODO
  while (1)
    {
      protocol_slidekeepalive ();

      protocol_sendpacket (dstpeer, PROTOCOL1_KA);
	  sleep(PROTOCOL_HOLEPUNCHINGTIME);
    }*/
}
