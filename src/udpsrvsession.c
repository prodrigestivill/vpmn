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

#include <pthread.h>
#include <string.h>
#include "udpsrvsession.h"
#include "debug.h"

struct udpsrvsession_l
{
  struct udpsrvsession_t *current;
  struct udpsrvsession_l *next;
};

int udpsrvsessions_len = 0;
struct udpsrvsession_l *udpsrvsessions;
pthread_mutex_t udpsrvsessions_mutex;

struct udpsrvsession_t *
udpsrvsession_search (char *s_addr, int s_port)
{
  struct udpsrvsession_l *cursession = udpsrvsessions;
  while (cursession != NULL)
    {
      if ((cursession->current != NULL)
	  && (cursession->current->s_port == s_port)
	  && (strcmp (s_addr, cursession->current->s_addr) == 0))
	{
	  return cursession->current;
	}
      cursession = cursession->next;
    }
  udpsrvsession_create (s_addr, s_port);
}

struct udpsrvsession_t *
udpsrvsession_create (char *s_addr, int s_port)
{

}

void
udpsrvsession_clean ()
{

}
