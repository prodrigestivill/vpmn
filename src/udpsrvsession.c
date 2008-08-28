/***************************************************************************
 *            udpsrvsession.c
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

#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "udpsrvsession.h"
#include "debug.h"

struct udpsrvsession_l
{
  struct udpsrvsession_s *current;
  struct udpsrvsession_l *next;
};

struct udpsrvsession_l *udpsrvsessions = NULL;
struct udpsrvsession_l *udpsrvsessions_last = NULL;
pthread_mutex_t udpsrvsessions_mutex = PTHREAD_MUTEX_INITIALIZER;

struct udpsrvsession_s *udpsrvsession_create(const struct sockaddr_in
                                             *source)
{
  struct udpsrvsession_s *newsession =
    malloc(sizeof(struct udpsrvsession_s));
  struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
  addr->sin_family = source->sin_family;
  addr->sin_addr.s_addr = source->sin_addr.s_addr;
  addr->sin_port = source->sin_port;
  newsession->addr = addr;
  newsession->peer = NULL;
  newsession->dtls_reading = 0;
  pthread_mutex_init(&newsession->dtls_mutex, NULL);
  pthread_mutex_init(&newsession->bioread_mutex, NULL);
  newsession->dtls = NULL;
  timeout_update(&newsession->timeout);
  return newsession;
}

struct udpsrvsession_s *udpsrvsession_search(const struct sockaddr_in
                                             *source)
{
  struct udpsrvsession_l *cursession;
  cursession = udpsrvsessions;
  while (cursession != NULL)
    {
      if ((cursession->current != NULL)
          && (source->sin_family == cursession->current->addr->sin_family)
          && (source->sin_port == cursession->current->addr->sin_port)
          && (source->sin_addr.s_addr ==
              cursession->current->addr->sin_addr.s_addr))
        return cursession->current;
      cursession = cursession->next;
    }
  return NULL;
}

struct udpsrvsession_s *udpsrvsession_searchcreate(const struct sockaddr_in
                                                   *source)
{
  struct udpsrvsession_l *cursession;
  int local_mutex = 0;
  if (udpsrvsessions == NULL)
    {
      pthread_mutex_lock(&udpsrvsessions_mutex);
      local_mutex = 1;
      if (udpsrvsessions != NULL)
        {
          pthread_mutex_unlock(&udpsrvsessions_mutex);
          local_mutex = 0;
        }
    }
  //Search the session
  cursession = udpsrvsessions;
  while (cursession != NULL)
    {
      if ((cursession->current != NULL)
          && (source->sin_family == cursession->current->addr->sin_family)
          && (source->sin_port == cursession->current->addr->sin_port)
          && (source->sin_addr.s_addr ==
              cursession->current->addr->sin_addr.s_addr))
        return cursession->current;
      if (cursession->next == NULL)
        {
          pthread_mutex_lock(&udpsrvsessions_mutex);
          local_mutex = 1;
          if (cursession->next != NULL)
            {
              pthread_mutex_unlock(&udpsrvsessions_mutex);
              local_mutex = 0;
            }
          else
            break;
        }
      cursession = cursession->next;
    }
  //Add new session
  if (local_mutex == 0)
    pthread_mutex_lock(&udpsrvsessions_mutex);
  cursession = malloc(sizeof(struct udpsrvsession_l));
  cursession->current = udpsrvsession_create(source);
  cursession->next = NULL;
  if (udpsrvsessions == NULL)
    udpsrvsessions = cursession;
  if (udpsrvsessions_last != NULL)
    udpsrvsessions_last->next = cursession;
  udpsrvsessions_last = cursession;
  pthread_mutex_unlock(&udpsrvsessions_mutex);
  return cursession->current;
}

void udpsrvsession_destroy(struct udpsrvsession_s *cursession)
{

}
