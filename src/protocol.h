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

#include <netinet/ip.h>
#include <bits/endian.h>
#include "router.h"

#define MAXKAPEERS 200

#define PROTOCOL1_V    1        //IP version for internal packets
#define PROTOCOL1_ID   0
#define PROTOCOL1_IDA  1        //ACK
#define PROTOCOL1_KA   2

/*-TOCHECK __attribute((packed)) */

struct protocol_addrpair_s
{
  uint32_t addr;
  uint16_t port;
} __attribute((packed));

struct protocol_netpair_s
{
  uint32_t addr;
  uint32_t netmask;
} __attribute((packed));

/* Structure implemented */
struct protocol_peer_s
{
  uint8_t len_net;
  uint8_t len_addr;
//struct protocol_netpair_s networks[];
//struct protocol_addrpair_s addrpairs[];
} __attribute((packed));

/* Protocol 1 empty */
struct protocol_1_s
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  unsigned int ihl:4;
  unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
  unsigned int version:4;
  unsigned int ihl:4;
#else
# error "Please fix <bits/endian.h>"
#endif
} __attribute((packed));

/* Protocol 1 Identifier */
struct protocol_1id_s
{
  struct protocol_1_s base;
  struct protocol_peer_s peer;
} __attribute((packed));

/* Protocol 1 Identifier ACK*/
struct protocol_1ida_s
{
  struct protocol_1_s base;
  uint8_t padding1;
  uint16_t padding2;
} __attribute((packed));

/* Protocol 1 Keep Alive
 * Structure implemented */
struct protocol_1ka_s
{
  struct protocol_1_s base;
  uint8_t len;
//struct protocol_peer_s peer[];
} __attribute((packed));

void protocol_init();
void protocol_recvpacket(const char *tunbuffer, const int tunbuffer_len,
                         struct udpsrvsession_s *session);
int protocol_sendframe(const char *buffer, const int buffer_len);
int protocol_sendpacket(struct udpsrvsession_s *session, const int type);
int protocol_processpeer(struct peer_s *peer,
                         struct protocol_peer_s *fragment, int max_size);
void protocol_maintainerthread();
int protocol_checknameconstraints(const struct peer_s *peer);

#endif                          /* _PROTOCOL_H */
