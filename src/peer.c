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

void
peer_add (struct peer_s *peer, struct udpsrvsession_s *session)
{
  //-TODO
}

struct peer_s *
peer_create ()
{
  struct peer_s *newpeer = malloc (sizeof (struct peer_s));
  newpeer->udpsrvsession = NULL;
  newpeer->addrs = NULL;
  newpeer->addrs_len = 0;
  newpeer->shared_networks = NULL;
  newpeer->shared_networks_len = 0;
  newpeer->recivack = 0;
  return newpeer;
}

void
peer_destroy (struct peer_s *oldpeer)
{
  int i;
  if (oldpeer == NULL)
	  return;
  //router_flush (oldpeer); Only by timeout function.
  udpsrvsession_destroy (oldpeer->udpsrvsession);
  if (oldpeer->shared_networks != NULL)
    free (oldpeer->shared_networks);
  if (oldpeer->addrs != NULL)
    free (oldpeer->addrs);
  free (oldpeer);
}
