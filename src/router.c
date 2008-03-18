/***************************************************************************
 *            router.c
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "config.h"
#include "udpsrvdtls.h"
#include "udpsrvsession.h"
#include "peer.h"

struct router_table_l
{
  struct in_network *network;
  struct peer_s *peer;
  struct router_table_l *next;
};

pthread_mutex_t router_table_mutex = PTHREAD_MUTEX_INITIALIZER;
struct router_table_l *router_table;
pthread_mutex_t router_expired_table_mutex = PTHREAD_MUTEX_INITIALIZER;
struct router_table_l **router_expired_table;
int router_expired_table_len = 0;

struct peer_s *router_searchdst(const struct in_addr *dst)
{
  struct router_table_l *current = router_table;
  while (current != NULL)
    {
      if ((dst->s_addr & current->network->netmask.s_addr) ==
          current->network->addr.s_addr)
        return current->peer;
      current = current->next;
    }
  return NULL;
}

int router_existroute(struct in_network *network)
{
  struct router_table_l *current = router_table;
  network->addr.s_addr = network->addr.s_addr & network->netmask.s_addr;
  while (current != NULL)
    {
      if ((network->addr.s_addr == current->network->addr.s_addr) &&
          (network->netmask.s_addr == current->network->netmask.s_addr))
        return 1;
      current = current->next;
    }
  return 0;
}

void router_addroute(struct in_network *network, struct peer_s *peer)
{
  struct router_table_l *current;
  struct router_table_l *current_last;
  struct router_table_l *newroute;
  network->addr.s_addr = network->addr.s_addr & network->netmask.s_addr;
  newroute = malloc(sizeof(struct router_table_l));
  newroute->network = network;
  newroute->peer = peer;
  //peer->shared_networks[peer->shared_networks_len] = network;
  //peer->shared_networks_len++;
  pthread_mutex_lock(&router_table_mutex);
  if (router_table == NULL)
    {
      newroute->next = NULL;
      router_table = newroute;
      pthread_mutex_unlock(&router_table_mutex);
      return;
    }
  current_last = NULL;
  current = router_table;
  while (current != NULL)
    {
      if (current->network->netmask.s_addr < network->netmask.s_addr)
        break;
      current_last = current;
      current = current->next;
    }
  newroute->next = current;
  current_last->next = newroute;
  pthread_mutex_unlock(&router_table_mutex);
  return;
}

void router_flushexpired()
{
  int i;
  if (router_expired_table_len < 1)
    return;
  pthread_mutex_lock(&router_expired_table_mutex);
  for (i = 0; i < router_expired_table_len; i++)
    {
      free(router_expired_table[i]);
    }
  free(router_expired_table);
  router_expired_table_len = 0;
  router_expired_table = NULL;
  pthread_mutex_unlock(&router_expired_table_mutex);
}

void router_flush(const struct peer_s *peer)
{
  struct router_table_l *current;
  struct router_table_l *current_last;
  if (router_table == NULL)
    return;
  pthread_mutex_lock(&router_table_mutex);
  pthread_mutex_lock(&router_expired_table_mutex);
  current = router_table;
  while (current != NULL && current->peer == peer)
    {
      router_expired_table_len++;
      router_expired_table =
        realloc(router_expired_table,
                router_expired_table_len *
                sizeof(struct router_table_l *));
      router_expired_table[router_expired_table_len - 1] = current;
      router_table = current->next;
      current = router_table;
    }
  current = router_table;
  current_last = NULL;
  while (current != NULL)
    {
      if (current->peer == peer)
        {
          current_last->next = current->next;
          router_expired_table_len++;
          router_expired_table =
            realloc(router_expired_table,
                    router_expired_table_len *
                    sizeof(struct router_table_l *));
          router_expired_table[router_expired_table_len - 1] = current;
          current = current_last->next;
        }
      else
        {
          current_last = current;
          current = current->next;
        }
    }
  pthread_mutex_unlock(&router_expired_table_mutex);
  pthread_mutex_unlock(&router_table_mutex);
  router_flushexpired();        //CAUTION
}

int router_checksrc(const struct in_addr *src, const struct peer_s *peer)
{
  int n;
  if (peer->shared_networks == NULL || peer->shared_networks_len < 1)
    return -1;
  for (n = 0; n < peer->shared_networks_len; n++)
    {
      if ((src->s_addr & peer->shared_networks[n].netmask.s_addr) ==
          peer->shared_networks[n].addr.s_addr)
        return 0;
    }
  return -2;
}
