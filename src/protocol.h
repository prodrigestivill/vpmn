/***************************************************************************
 *            protocol.h
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

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "router.h"

#define MAXKAPEERS 200

#define PROTOCOL1_ID   0x01
#define PROTOCOL1_KA   0x02
#define PROTOCOL1_IDKA 0x03

struct protocol_addrpair_s
{
  uint32_t addr;
  uint16_t port;
};

struct protocol_netpair_s
{
  uint32_t addr;
  uint32_t netmask;
};

/* Protocol 1 Identifier
 * Structure implemented */
struct protocol_1id_s
{
  char packetid;
  char len_addr;
  char len_net;
  uint16_t udpport;
//uint32_t addrs[];
//struct protocol_netpair_s networks[];
};

/* Protocol 1 Keep Alive
 * Structure implemented */
struct protocol_1ka_s
{
  char packetid;
  char len;
//struct protocol_addrpair_s addrs[];
};

void protocol_recvpacket (const char *tunbuffer, const int tunbuffer_len,
			  struct udpsrvsession_s *session);
int protocol_sendframe (const char *buffer, const int buffer_len);
int protocol_sendpacket (const struct peer_s *dstpeer, const int type);
void protocol_maintainerthread ();

#endif /* _PROTOCOL_H */
