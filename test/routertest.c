/***************************************************************************
 *            routertest.c
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../src/config.h"
#include "../src/debug.h"
#include "../src/peer.h"
#include "../src/router.h"

struct peer_s *addpeer(const char *ip, int port)
{
  struct peer_s *peer = peer_create();
  struct sockaddr_in *spaddr = malloc(sizeof(struct sockaddr_in));
  inet_aton(ip, &spaddr->sin_addr);
  spaddr->sin_port = htons(port);
  peer->udpsrvsession = NULL;
#if 0
  peer->udpsrvsessions = calloc(1, sizeof(struct udpsrvsession *));
  peer->udpsrvsessions[0] = udpsrvsession_search(spaddr);
  peer->udpsrvsessions_len = 1;
#endif
  return peer;
}

void addpeerroute(struct peer_s *peer, const char *ip, const char *nm)
{
  struct in_network *network = malloc(sizeof(struct in_network));
  inet_aton(ip, &network->addr);
  inet_aton(nm, &network->netmask);
  router_addroute(network, peer);
}

struct peer_s *searchdst(const char *ip)
{
  struct in_addr *dst = malloc(sizeof(struct in_addr));
  inet_aton(ip, dst);
  return router_searchdst(dst);
}

int main(int argc, char **argv)
{
  struct peer_s *peer;
  if (argc<2) {
    log_error ("I need the configuration file\n");
    return 1;
  }
  config_load(argv[1]);

  peer = addpeer("10.0.5.1", 1901);
  addpeerroute(peer, "10.2.0.1", "255.255.255.255");
  addpeerroute(peer, "10.5.1.1", "255.255.255.0");

  peer = addpeer("10.0.5.2", 1902);
  addpeerroute(peer, "10.2.0.2", "255.255.255.255");
  addpeerroute(peer, "10.5.2.2", "255.255.0.0");

  peer = addpeer("10.0.5.3", 1903);
  addpeerroute(peer, "10.2.0.3", "255.255.255.255");
  addpeerroute(peer, "10.3.3.3", "255.255.0.0");
  addpeerroute(peer, "10.5.3.3", "255.255.255.0");

  char *test = "10.2.0.3";
  log_debug("%s", test);
  log_debug("-> %d\n",
            ntohs(searchdst(test)->udpsrvsessions[0]->addr->sin_port));
  test = "10.3.10.2";
  log_debug("%s", test);
  log_debug("-> %d\n",
            ntohs(searchdst(test)->udpsrvsessions[0]->addr->sin_port));
  test = "10.5.1.250";
  log_debug("%s", test);
  log_debug("-> %d\n",
            ntohs(searchdst(test)->udpsrvsessions[0]->addr->sin_port));
  test = "10.5.20.2";
  log_debug("%s", test);
  log_debug("-> %d\n",
            ntohs(searchdst(test)->udpsrvsessions[0]->addr->sin_port));
  return 0;
}

void protocol_sendroutes(const struct peer_s *dstpeer)
{
  log_debug("Sending routes... (not implemented)");
}

void udpsrvdtls_init()
{
}

int
udpsrvdtls_loadcerts(const char *cafile, const char *certfile,
                     const char *pkeyfile)
{
  return 0;
}
