/***************************************************************************
 *            protocol.h
 *
 *  Tue Feb 19 15:14:07 2008
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

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "router.h"
#define MAX_PROTOCOL_ROUTES 10

#define UDPSRVSESSION 0

#define PROTOCOL1_ID 0x01
#define PROTOCOL1_KA 0x02

struct protocol_route
{
  char p;
  struct in_network routes[MAX_PROTOCOL_ROUTES];
};

void protocol_recvpacket (const char *tunbuffer, const int tunbuffer_len,
			  void *session, const int sessiontype);
void protocol_sendframe (const char *buffer, const int buffer_len);
void protocol_sendroutes (const struct peer_t *dstpeer);

#endif /* _PROTOCOL_H */
