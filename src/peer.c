/***************************************************************************
 *            peer.c
 *
 *  Tue Feb  7 00:24:22 2008
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

#include <stdlib.h>
#include "router.h"
#include "peer.h"

struct peer_t *
peer_create ()
{
  struct peer_t *newpeer = malloc (sizeof (struct peer_t));
  newpeer->udpsrvsession = NULL;
  //newpeer->tcpsrvsession = NULL;
  newpeer->addrs_len = 0;
  newpeer->shared_networks_len = 0;
  return newpeer;
}

void
peer_destroy (struct peer_t *oldpeer)
{
  router_flush (oldpeer);
  free (oldpeer);
}
