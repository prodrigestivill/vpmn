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

//-TODO MUEXES
struct protocol_1id_s *protocol_v1idka;
int protocol_v1idka_maxlen;
struct protocol_1id_s *protocol_v1id;
int protocol_v1id_len;
struct protocol_1ka_s *protocol_v1ka;
int protocol_v1ka_len;
int protocol_v1ka_pos;

void
protocol_recvpacket (const char *buffer, const int buffer_len,
		     struct udpsrvsession_s *session)
{
  int i, begin = 0;
  struct sockaddr_in saddr;
  struct peer_s *peer = NULL;

  if (buffer_len < 4)
    return;
  if (session != NULL)
    peer = session->peer;
  if ((buffer[begin] & 0xF0) == 0x40)	//IPv4 packet
    {
      if (peer == NULL || buffer_len < 20)
	return;
      //DST: Check if it is an interested packet for own host
      saddr.sin_addr.s_addr = (buffer[16]) | (buffer[17] << 8) |
	(buffer[18] << 16) | (buffer[19] << 24);
      if (router_checksrc (&saddr.sin_addr, &tun_selfpeer) != 0)
	return;
      //SRC: Check permisions
      saddr.sin_addr.s_addr = (buffer[12]) | (buffer[13] << 8) |
	(buffer[14] << 16) | (buffer[15] << 24);
      if (router_checksrc (&saddr.sin_addr, peer) != 0)
	return;
      tundev_write (buffer, buffer_len);
    }
//else if ((buffer[begin] & 0xF0) == 0x60) //IPv6 Packet
  else				//Internal packets
    {
      if (buffer[begin] == PROTOCOL1_ID || buffer[begin] == PROTOCOL1_IDKA)
	{
	  /*if (((buffer[begin + 1] * sizeof (struct protocol_addrpair_t)) +
	     +...) > buffer_len)
	     return; */
	  if (peer == NULL)
	    {
	      //create peer
	    }
	  if (buffer[begin] == PROTOCOL1_ID)
	    protocol_sendpacket (peer, PROTOCOL1_IDKA);
	  //Packets are combinable ID+KA
	  i = begin + 2 + sizeof (struct protocol_addrpair_s) +
	    (buffer[begin + 1] * sizeof (struct protocol_netpair_s));
	  if (i + 2 < buffer_len)
	    begin = i;
	}
      if (buffer[begin] == PROTOCOL1_KA)
	{
	  if (buffer[begin + 1] == 0
	      ||
	      ((buffer[begin + 1] * sizeof (struct protocol_addrpair_s)) +
	       begin + 2) > buffer_len)
	    return;
	  for (i = begin + 2; i < buffer_len;
	       i += sizeof (struct protocol_addrpair_s))
	    {
	      saddr.sin_addr.s_addr =
		((struct protocol_addrpair_s *) &buffer[i])->addr;
	      saddr.sin_port =
		((struct protocol_addrpair_s *) &buffer[i])->port;
	      //peer_add(&addr);
	    }
	}
    }
}

int
protocol_sendraw (const char *buffer, const int buffer_len,
		  const struct peer_s *dstpeer)
{
  if (dstpeer->udpsrvsessions_len > 0)
    {
      udpsrvdtls_write (buffer, buffer_len, dstpeer->udpsrvsessions[0]);
      return 0;
    }
  return -1;
}

int
protocol_sendframe (const char *buffer, const int buffer_len)
{
  struct in_addr addr;
  struct peer_s *dstpeer = NULL;
  if (buffer_len < 20)
    return -1;
  //Check for IPv4
  if ((buffer[0] & 0xF0) == 0x40)
    {
      //SRC: Valid source?
      addr.s_addr = (buffer[12]) | (buffer[13] << 8) |
	(buffer[14] << 16) | (buffer[15] << 24);
      if (router_checksrc (&addr, &tun_selfpeer) == 0)
	{
	  //DST: Where to send?
	  addr.s_addr = (buffer[16]) | (buffer[17] << 8) |
	    (buffer[18] << 16) | (buffer[19] << 24);
	  //-TODO: CHECK BROADCAST
	  dstpeer = router_searchdst (&addr);
	}
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
    return -1;
  return protocol_sendraw (buffer, buffer_len, dstpeer);
}

int
protocol_sendpacket (const struct peer_s *dstpeer, const int type)
{
  char *packet;
  int packet_len = -1;
  switch (type)
    {
    case PROTOCOL1_ID:
      packet = (char *) protocol_v1id;
      packet_len = protocol_v1id_len;
      break;
    case PROTOCOL1_KA:
      packet = (char *) protocol_v1ka;
      packet_len = protocol_v1ka_len;
      break;
    case PROTOCOL1_IDKA:
      //Both are together
      packet = (char *) protocol_v1idka;
      packet_len = protocol_v1id_len + protocol_v1ka_len;
      break;
    }
  if (packet_len < 1)
    return -1;
  return protocol_sendraw (packet, packet_len, dstpeer);
}

void
protocol_slidekeepalive ()
{
  int i;
  if (protocol_v1id_len + protocol_v1ka_len < protocol_v1idka_maxlen)
    protocol_v1ka_pos = 0;
  i = udpsrvsession_dumpsocks (protocol_v1ka + sizeof (struct protocol_1ka_s),
			       protocol_v1idka_maxlen - protocol_v1id_len,
			       protocol_v1ka_pos, MAXKAPEERS);
  protocol_v1ka->packetid = PROTOCOL1_KA;
  protocol_v1ka->len = i;
  protocol_v1ka_len =
    sizeof (struct protocol_1ka_s) + i * sizeof (struct protocol_addrpair_s);
  protocol_v1ka_pos += i;
}

void
protocol_init ()
{
  int i;
  protocol_v1id_len = sizeof (struct protocol_1id_s) +
    tun_selfpeer.addrs_len * sizeof (uint32_t) +
    tun_selfpeer.shared_networks_len * sizeof (struct protocol_netpair_s);
  protocol_v1id = malloc (protocol_v1id_len);
  protocol_v1id->packetid = PROTOCOL1_ID;
  if (tun_selfpeer.shared_networks_len < 0)
    return;
  if (tun_selfpeer.addrs_len < 256)
    protocol_v1id->len_addr = tun_selfpeer.addrs_len;
  else
    protocol_v1id->len_addr = 255;
  if (tun_selfpeer.shared_networks_len < 256)
    protocol_v1id->len_net = tun_selfpeer.shared_networks_len;
  else
    protocol_v1id->len_net = 255;
  protocol_v1id->udpport = htons (port_udp);
  for (i = 0; i < protocol_v1id->len_addr; i++)
    {
      memcpy (protocol_v1id + sizeof (struct protocol_1id_s) +
	      i * sizeof (uint32_t), &tun_selfpeer.addrs[i].s_addr,
	      sizeof (uint32_t));
    }
  for (i = 0; i < protocol_v1id->len_net; i++)
    {
      memcpy (protocol_v1id + sizeof (struct protocol_1id_s) +
	      protocol_v1id->len_addr * sizeof (uint32_t) +
	      i * sizeof (struct protocol_netpair_s),
	      &tun_selfpeer.shared_networks[i].addr.s_addr,
	      sizeof (uint32_t));
      memcpy (protocol_v1id + sizeof (struct protocol_1id_s) +
	      protocol_v1id->len_addr * sizeof (uint32_t) +
	      i * sizeof (struct protocol_netpair_s) + sizeof (uint32_t),
	      &tun_selfpeer.shared_networks[i].netmask.s_addr,
	      sizeof (uint32_t));
    }
  protocol_v1ka_len = sizeof (struct protocol_1ka_s) +
    MAXKAPEERS * sizeof (struct protocol_addrpair_s);
  protocol_v1idka_maxlen = protocol_v1id_len + protocol_v1ka_len;
  protocol_v1idka = malloc (protocol_v1idka_maxlen);
  memcpy (protocol_v1idka, protocol_v1id, protocol_v1id_len);
  protocol_v1idka->packetid = PROTOCOL1_IDKA;
  protocol_v1ka = (struct protocol_1ka_s *) protocol_v1idka +
    protocol_v1id_len;
  protocol_v1ka_pos = 0;
  protocol_slidekeepalive ();
}

void
protocol_maintainerthread ()
{
//struct peer_s *dstpeer;
  protocol_init ();
/*-TODO
  while (1)
    {
      protocol_slidekeepalive ();

      protocol_sendpacket (dstpeer, PROTOCOL1_KA);
	  sleep(PROTOCOL_HOLEPUNCHINGTIME);
    }*/
}
