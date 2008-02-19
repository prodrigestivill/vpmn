/***************************************************************************
 *            protocol.c
 *
 *  Tue Feb 19 15:13:39 2008
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
#include "config.h"
#include "protocol.h"
#include "peer.h"
#include "router.h"
#include "udpsrvdtls.h"

void
protocol_sendroutes (const struct peer_t *peer)
{
  int len;
  struct protocol_route table;
  table.p = '\0';
  if (tunaddr_networks_len > MAX_PROTOCOL_ROUTES)
    len = MAX_PROTOCOL_ROUTES;
  else
    len = tunaddr_networks_len;
  memcpy (&table.routes, &tunaddr_networks, len * sizeof (struct in_network));
  if (peer->udpsrvsession != NULL)
    {
      udpsrvdtls_write ((char *) &table, sizeof (table), peer->udpsrvsession);
    }
}
