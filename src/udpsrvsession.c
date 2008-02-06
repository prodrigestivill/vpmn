/***************************************************************************
 *            udpsrvsession.c
 *
 *  Tue Feb  6 12:03:15 2008
 *  Copyright  2008  Pau Rodriguez-Estivill
 *  <prodrigestivill@gmail.com>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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

#include "udpsrvsession.h"
#include "debug.h"

int udpsrvsession_len = 0;
struct udpsrvsession_t udpsrvsessions[];

udpsrvsession_t
udpsrvsession_search (struct *sockaddr_in addr, socklen_t * addr_len)
{
  int i;
  for (i = 0; i < udpsrvsession_len; i++)
    {
    }
  if (i >= udpsrvsession_len)
    udpsrvsession_create (&addr, &addr_len);
}

udpsrvsession_t
udpsrvsession_create (struct *sockaddr_in addr, socklen_t * addr_len)
{

}

void
udpsrvsession_clean ()
{
  int i;
  for (i = 0; i < udpsrvsession_len; i++)
    {
      if (timeout <= 0)		//repair time.

	}
