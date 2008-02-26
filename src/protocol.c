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
struct protocol_1id_s *protocol_v1id;
int protocol_v1id_len;
struct protocol_1_s *protocol_v1ida;
int protocol_v1ida_len;
struct protocol_1ka_s *protocol_v1ka;
int protocol_v1ka_len;
int protocol_v1ka_maxlen;
int protocol_v1ka_pos;

void
protocol_recvpacket (const char *buffer, const int buffer_len,
		     struct udpsrvsession_s *session)
{
  int i, begin = 0;
  struct sockaddr_in saddr;
  struct peer_s *peer = NULL;
  struct udpsrvsession_s *newsession = NULL;

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
      if (buffer[begin] == PROTOCOL1_ID)
	{
	  /*if (((buffer[begin + 1] * sizeof (struct protocol_addrpair_t)) +
	     +...) > buffer_len)
	     return; */
	  /*
	  if (peer == NULL)
	    {
	      peer = peer_create ();
	      peer->addrs_len =
		((struct protocol_1id_s *) &buffer[begin])->peer.len_addr;
	      peer->shared_networks_len =
		((struct protocol_1id_s *) &buffer[begin])->peer.len_net;
	      peer->addrs =
		calloc (peer->addrs_len, sizeof (struct sockaddr_in));
	      peer->shared_networks =
		calloc (peer->shared_networks_len,
			sizeof (struct in_network));
	      for (i = 0; i < peer->addrs_len; i++)
		{
		  peer->addrs[i].sin_family = AF_INET;
		  peer->addrs[i].sin_port =
		    ((struct protocol_1id_s *) &buffer[begin])->udpport;
		  memcpy (&peer->addrs[i].sin_addr.s_addr,
			  buffer + sizeof (struct protocol_1id_s) +
			  i * sizeof (uint32_t), sizeof (uint32_t));

		}
	      for (i = 0; i < peer->shared_networks_len; i++)
		{
		  memcpy (&peer->shared_networks[i].addr.s_addr,
			  buffer + sizeof (struct protocol_1id_s) +
			  peer->addrs_len * sizeof (uint32_t) +
			  i * sizeof (struct protocol_netpair_s),
			  sizeof (uint32_t));
		  memcpy (&peer->shared_networks[i].addr.s_addr,
			  buffer + sizeof (struct protocol_1id_s) +
			  peer->addrs_len * sizeof (uint32_t) +
			  i * sizeof (struct protocol_netpair_s) +
			  sizeof (uint32_t), sizeof (uint32_t));
		}
	      if (peer_compare (&tun_selfpeer, peer))
		{
		  peer_destroy (peer);
		  return;
		}
	      peer_add (peer, session);
	    }
	  if (buffer[begin] == PROTOCOL1_ID)
	    protocol_sendpacket (session, PROTOCOL1_IDKA);
	  //Packets are combinable ID+KA
	  i = begin + sizeof (struct protocol_1id_s) +
	    ((struct protocol_1id_s *) &buffer[begin])->len_addr *
	    sizeof (uint32_t) +
	    ((struct protocol_1id_s *) &buffer[begin])->len_net *
	    sizeof (struct protocol_netpair_s);
	  if (i + 2 < buffer_len)
	    begin = i;
	}
	*/
      if (buffer[begin] == PROTOCOL1_KA)
	{
	/*
	  if (((struct protocol_1ka_s *) &buffer[begin])->len == 0
	      || (begin + sizeof (struct protocol_1ka_s) +
		  (((struct protocol_1ka_s *) &buffer[begin])->len *
		   sizeof (struct protocol_addrpair_s))) > buffer_len)
	    return;
	  saddr.sin_family = AF_INET;
	  for (i = begin + sizeof (struct protocol_1ka_s); i < buffer_len;
	       i += sizeof (struct protocol_addrpair_s))
	    {
	      saddr.sin_addr.s_addr =
		((struct protocol_addrpair_s *) &buffer[i])->addr;
	      saddr.sin_port =
		((struct protocol_addrpair_s *) &buffer[i])->port;
	      //-TODO: Control more exactly the states
	      newsession = udpsrvsession_search (&saddr);
	      if (newsession == NULL)
		{
		  newsession = udpsrvsession_searchcreate (&saddr)
		    protocol_sendpacket (newsession, PROTOCOL1_ID);
		}
	      if (newsession->peer != NULL)
		protocol_sendpacket (newsession, PROTOCOL1_IDKA);
		*/
	    }
	}
    }
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
  if (buffer_len > 0 && dstpeer->udpsrvsession != NULL)
    {
      udpsrvdtls_write (buffer, buffer_len, dstpeer->udpsrvsession);
      return 0;
    }
  return -1;
}

int
protocol_sendpacket (struct udpsrvsession_s *session, const int type)
{
  char *packet = NULL;
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
    }
  if (packet_len < 1 && session != NULL)
    {
      udpsrvdtls_write (packet, packet_len, session);
      return 0;
    }
  return -1;
}

void
protocol_slidekeepalive ()
{
/*
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
*/
}

void
protocol_init ()
{
  struct protocol_addrpair_s *addrpair;
  struct protocol_netpair_s *netpair;
  int i;
  protocol_v1ida_len = sizeof (struct protocol_1_s);
  protocol_v1ida = malloc (protocol_v1ida_len);
  protocol_v1ida->packetid = PROTOCOL1_IDA;
  protocol_v1id_len = sizeof (struct protocol_1id_s) +
    tun_selfpeer.shared_networks_len * sizeof (struct protocol_netpair_s) +
    tun_selfpeer.addrs_len * sizeof (struct protocol_addrpair_s);
  protocol_v1id = malloc (protocol_v1id_len);
  protocol_v1id->packetid = PROTOCOL1_ID;
  if (tun_selfpeer.shared_networks_len < 0)
    return;
  if (tun_selfpeer.addrs_len < 256)
    protocol_v1id->peer.len_addr = tun_selfpeer.addrs_len;
  else
    protocol_v1id->peer.len_addr = 255;
  if (tun_selfpeer.shared_networks_len < 256)
    protocol_v1id->peer.len_net = tun_selfpeer.shared_networks_len;
  else
    protocol_v1id->peer.len_net = 255;
  for (i = 0; i < protocol_v1id->peer.len_net; i++)
    {
      netpair =
	(struct protocol_netpair_s *) protocol_v1id +
	sizeof (struct protocol_1id_s) +
	i * sizeof (struct protocol_netpair_s);
      memcpy (&netpair->addr, &tun_selfpeer.shared_networks[i].addr.s_addr,
	      sizeof (uint32_t));
      memcpy (&netpair->netmask,
	      &tun_selfpeer.shared_networks[i].netmask.s_addr,
	      sizeof (uint32_t));
    }
  for (i = 0; i < protocol_v1id->peer.len_addr; i++)
    {
      addrpair =
	(struct protocol_addrpair_s *) protocol_v1id +
	sizeof (struct protocol_1id_s) +
	protocol_v1id->peer.len_net * sizeof (struct protocol_netpair_s) +
	i * sizeof (struct protocol_addrpair_s);
      addrpair->port = tun_selfpeer.addrs[i].sin_port;
      memcpy (&addrpair->addr, &tun_selfpeer.addrs[i].sin_addr.s_addr,
	      sizeof (uint32_t));
    }
  protocol_v1ka_maxlen =
    sizeof (struct protocol_1ka_s) +
    MAXKAPEERS * (sizeof (struct protocol_peer_s) +
		  sizeof (struct protocol_netpair_s) +
		  sizeof (struct protocol_netpair_s));
  protocol_v1ka = malloc (protocol_v1ka_maxlen);
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
