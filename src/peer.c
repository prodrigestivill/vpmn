/***************************************************************************
 *            peer.c
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
#include "router.h"
#include "peer.h"

struct peer_s *
peer_create ()
{
  struct peer_s *newpeer = malloc (sizeof (struct peer_s));
  newpeer->udpsrvsessions = NULL;
  newpeer->udpsrvsessions_len = 0;
  //newpeer->tcpsrvsession = NULL;
  newpeer->addrs_len = 0;
  newpeer->shared_networks_len = 0;
  return newpeer;
}

void
peer_destroy (struct peer_s *oldpeer)
{
  router_flush (oldpeer);
  free (oldpeer);
}

int
peer_compare (struct peer_s *peer1, struct peer_s *peer2)
{
  int i;
  if (peer1->addrs_len != peer2->addrs_len)
    return -1;
  if (peer1->shared_networks_len != peer2->shared_networks_len)
    return -1;
  for (i = 0; i < peer1->addrs_len; i++)
    if (peer1->addrs[i].s_addr != peer2->addrs[i].s_addr)
      return -1;
  for (i = 0; i < peer1->shared_networks_len; i++)
    if (peer1->shared_networks[i].addr.s_addr !=
	peer2->shared_networks[i].addr.s_addr
	|| peer1->shared_networks[i].netmask.s_addr !=
	peer2->shared_networks[i].netmask.s_addr)
      return -1;
  return 0;
}
