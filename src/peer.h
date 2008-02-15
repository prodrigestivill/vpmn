/***************************************************************************
 *            peer.h
 *
 *  Tue Feb  6 12:03:15 2008
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

#ifndef _PEER_H
#define _PEER_H

#include "udpsrvsession.h"

struct peer_t
{
  struct udpsrvsession_t *udpsrvsession;
//struct tcpsrvsession_t *tcpsrvsession;
  /*networks owned */
};

struct peer_t *peer_create ();
void peer_destroy (struct peer_t *oldpeer);

#endif /* _PEER_H */