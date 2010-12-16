/***************************************************************************
 *            protocoltest.c
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

#include "../src/protocol.h"
#include "../src/debug.h"
#include "../src/peer.h"
#include "../src/config.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct protocol_1id_s *protocol_v1id;
int protocol_v1id_len;
struct protocol_1_s protocol_v1ida;
#define protocol_v1ida_len sizeof (struct protocol_1_s);
struct protocol_1ka_s *protocol_v1ka;
int protocol_v1ka_len;
int protocol_v1ka_maxlen;
int protocol_v1ka_pos;

int main(int argc, char **argv)
{
  int i;
  struct peer_s *peer;
  if (argc<2) {
    log_error ("I need the configuration file\n");
    return 1;
  }
  peer = peer_create ();
  config_load(argv[1]);
  protocol_init();
  for (i = 0; i < protocol_v1id_len; i++)
    log_debug("%d ", *((char *) protocol_v1id + i));
  i = protocol_processpeer(peer, &protocol_v1id->peer, protocol_v1id_len -
		   sizeof(struct protocol_1id_s) + sizeof(struct protocol_peer_s));
  if (i < 0)
    return 1;
  log_debug("\n%d\n", i);
//peer = &tun_selfpeer;
  for (i = 0; i < peer->addrs_len; i++)
    {
      log_info("%s:", inet_ntoa(peer->addrs[i].sin_addr));
      log_info("%d\n", ntohs(peer->addrs[i].sin_port));
    }
  for (i = 0; i < peer->shared_networks_len; i++)
    {
      log_info("%s:", inet_ntoa(peer->shared_networks[i].addr));
      log_info("%s\n", inet_ntoa(peer->shared_networks[i].netmask));
    }
  return 0;
}

void udpsrvdtls_init()
{
}

int
udpsrvdtls_loadcerts(const char *cafile, const char *certfile,
                     const char *pkeyfile)
{
  log_error("Not implemented\n");
  return 0;
}

int
udpsrvdtls_write(const char *buffer, const int buffer_len,
                 struct udpsrvsession_s *session)
{
  log_error("Not implemented\n");
  return 0;
}

int tundev_write(const void *buf, const int count)
{
  log_error("Not implemented\n");
  return 0;
}
