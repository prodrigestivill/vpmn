/***************************************************************************
 *            peer.h
 *
 *  Copyright  2008  Pau Rodriguez-Estivill
 *  <prodrigestivill@gmail.com>
 ****************************************************************************/

/* This program is free software: you can redistribute it and/or modify
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

#ifndef _PEER_H
#define _PEER_H

#include <netinet/in.h>
#include "router.h"
#include "udpsrvsession.h"

#define PEER_STAT_NEW 0
#define PEER_STAT_ID  1		//IDENTIFIED
#define PEER_STAT_TO  2		//TIMEOUT

struct peer_t
{
  struct udpsrvsession_t *udpsrvsession;
//struct tcpsrvsession_t *tcpsrvsession;
  struct sockaddr_in **addrs;
  int addrs_len;
  struct in_network *shared_networks;
  int shared_networks_len;
  int stat;
};

struct peer_t *peer_create ();
void peer_destroy (struct peer_t *oldpeer);

#endif /* _PEER_H */
