#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../src/config.h"
#include "../src/debug.h"
#include "../src/peer.h"
#include "../src/router.h"

struct peer_t *
addpeer (const char *ip, int port)
{
  struct peer_t *peer = peer_create ();
  struct sockaddr_in *spaddr = malloc(sizeof(struct sockaddr_in));
  inet_aton (ip, &spaddr->sin_addr);
  spaddr->sin_port = htons (port);
  peer->udpsrvsession = udpsrvsession_search (spaddr);
  return peer;
}

void
addpeerroute (struct peer_t *peer, const char *ip, const char *nm)
{
  struct in_network *network = malloc (sizeof (struct in_network));
  inet_aton (ip, &network->addr);
  inet_aton (nm, &network->netmask);
  router_addroute (network, peer);
}

struct peer_t *
searchdst (const char *ip)
{
  struct in_addr *dst = malloc(sizeof(struct in_addr));
  inet_aton (ip, dst);
  return router_searchdst(dst);
}

void
main ()
{
  struct peer_t *peer;
  config_load ();

  peer = addpeer ("10.0.5.1", 1901);
  addpeerroute (peer, "10.2.0.1", "255.255.255.255");
  addpeerroute (peer, "10.5.1.1", "255.255.255.0");

  peer = addpeer ("10.0.5.2", 1902);
  addpeerroute (peer, "10.2.0.2", "255.255.255.255");
  addpeerroute (peer, "10.5.2.2", "255.255.0.0");

  peer = addpeer ("10.0.5.3", 1903);
  addpeerroute (peer, "10.2.0.3", "255.255.255.255");
  addpeerroute (peer, "10.3.3.3", "255.255.0.0");
  addpeerroute (peer, "10.5.3.3", "255.255.255.0");
  
  char *test = "10.2.0.3";
  log_debug("%s", test);
  log_debug("-> %d\n", ntohs(searchdst(test)->udpsrvsession->addr->sin_port));
  test = "10.3.10.2";
  log_debug("%s", test);
  log_debug("-> %d\n", ntohs(searchdst(test)->udpsrvsession->addr->sin_port));
  test = "10.5.1.250";
  log_debug("%s", test);
  log_debug("-> %d\n", ntohs(searchdst(test)->udpsrvsession->addr->sin_port));
  test = "10.5.20.2";
  log_debug("%s", test);
  log_debug("-> %d\n", ntohs(searchdst(test)->udpsrvsession->addr->sin_port));
}
