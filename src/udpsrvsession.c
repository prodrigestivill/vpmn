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

#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "udpsrvsession.h"
#include "debug.h"

struct udpsrvsession_l
{
  struct udpsrvsession_t *current;
  struct udpsrvsession_l *next;
};

int udpsrvsessions_len = 0;
struct udpsrvsession_l *udpsrvsessions = NULL;
struct udpsrvsession_l *udpsrvsessions_last = NULL;
pthread_mutex_t udpsrvsessions_mutex = PTHREAD_MUTEX_INITIALIZER;

struct udpsrvsession_t *
udpsrvsession_search (struct sockaddr_in *source)
{
  int sin_size = sizeof (struct sockaddr_in);
  struct udpsrvsession_l *cursession;
  int local_mutex = 0;
  if (udpsrvsessions == NULL)
    {
      pthread_mutex_lock (&udpsrvsessions_mutex);
      local_mutex = 1;
      if (udpsrvsessions != NULL)
	{
	  pthread_mutex_unlock (&udpsrvsessions_mutex);
	  local_mutex = 0;
	}
    }
  //Search the session
  cursession = udpsrvsessions;
  while (cursession != NULL)
    {
      if ((cursession->current != NULL)
	  && (memcmp (source, cursession->current->addr, sin_size) == 0))
	{
	  return cursession->current;
	}
      if (cursession->next == NULL)
	{
	  pthread_mutex_lock (&udpsrvsessions_mutex);
	  local_mutex = 1;
	  if (cursession->next != NULL)
	    {
	      pthread_mutex_unlock (&udpsrvsessions_mutex);
	      local_mutex = 0;
	    }
	  else
	    break;
	}
      cursession = cursession->next;
    }
  //Add new session
  if (local_mutex == 0)
    pthread_mutex_lock (&udpsrvsessions_mutex);
  cursession = malloc (sizeof (struct udpsrvsession_l));
  cursession->current = udpsrvsession_create (source);
  cursession->next = NULL;
  if (udpsrvsessions == NULL)
    udpsrvsessions = cursession;
  if (udpsrvsessions_last != NULL)
    udpsrvsessions_last->next = cursession;
  udpsrvsessions_last = cursession;
  pthread_mutex_unlock (&udpsrvsessions_mutex);
  return cursession->current;
}

struct udpsrvsession_t *
udpsrvsession_create (struct sockaddr_in *source)
{
  struct udpsrvsession_t *newsession =
    malloc (sizeof (struct udpsrvsession_t));
  newsession->addr = source;
  newsession->fd = udpsrvsessions_len++;
  newsession->peer = peer_create ();
  newsession->peer->udpsrvsession = newsession;
  udpsrvsession_update_timeout (newsession);
  return newsession;
}

void
udpsrvsession_clean ()
{

}

void
udpsrvsession_update_timeout (struct udpsrvsession_t *cursession)
{
  if (cursession != NULL)
    cursession->timeout = 0;
}
