/***************************************************************************
 *            router.h
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

#ifndef _ROUTER_H
#define _ROUTER_H

#include <netinet/in.h>

struct in_network
{
  struct in_addr addr;
  struct in_addr netmask;
};

struct peer_t *router_searchdst (const struct in_addr *dst);
//struct peer_t *router_searchdst6 (struct in6_addr *dst);
int router_checksrc (const struct in_addr *src, const struct peer_t *peer);
//int router_checksrc6 (struct in6_addr *src);
void router_addroute (struct in_network *network, struct peer_t *peer);
void router_flush (const struct peer_t *peer);

#endif /* _ROUTER_H */
