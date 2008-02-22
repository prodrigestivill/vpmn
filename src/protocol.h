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
#define MAX_PROTOCOL_ROUTES 10

#define NOSESSION -1
#define UDPSRVSESSION 0

#define PROTOCOL1_ID 0x01
#define PROTOCOL1_KA 0x02

void protocol_recvpacket (const char *tunbuffer, const int tunbuffer_len,
			  void *session, const int sessiontype);
int protocol_sendframe (const char *buffer, const int buffer_len);
int protocol_sendpacket (const struct peer_t *dstpeer, const int type);
void protocol_maintainerthread ();

#endif /* _PROTOCOL_H */
