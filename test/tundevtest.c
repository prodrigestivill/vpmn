/***************************************************************************
 *            tundevtest.c
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

#include "../src/config.h"
#include "../src/debug.h"
#include "../src/tundev.h"
#include "../src/udpsrvsession.h"
#include "../src/srv.h"

int main()
{
  config_load();
  if (tundev_initdev() < 0)
    {
      log_error("Could not create the interface.\n");
      return -1;
    }
  tunsrv();
  return 0;
}

void protocol_sendframe(const char *buffer, const int buffer_len)
{
  struct in_addr src4, dst4;
  //Check for IPv4
  if ((buffer[0] & 0xF0) == 0x40)
    {
      src4.s_addr = (buffer[12]) | (buffer[13] << 8) |
        (buffer[14] << 16) | (buffer[15] << 24);
      dst4.s_addr = (buffer[16]) | (buffer[17] << 8) |
        (buffer[18] << 16) | (buffer[19] << 24);
      if (router_checksrc(&src4, &tun_selfpeer) == 0)
        {
          log_debug("%s", inet_ntoa(src4));
          log_debug("->%s\n", inet_ntoa(dst4));
        }
      else
        log_error("Invalid source.\n");
    }
  //Check for IPv6
  else if ((buffer[0] & 0xF0) == 0x60)

    {
      log_error("IPv6 not implemented.\n");
    }
  else
    log_error("Unknow protocol not implemented.\n");
}

void
protocol_recvpacket(const char *tunbuffer, const int tunbuffer_len,
                    void *session, const int sessiontype)
{
  log_debug("Recive packet... (not implemented)");
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

struct udpsrvsession_s *udpsrvsession_search(const struct sockaddr_in
                                             *source)
{
  struct udpsrvsession_s *newsession =
    malloc(sizeof(struct udpsrvsession_s));
  newsession->addr = source;
  newsession->peer = peer_create();
  newsession->peer->udpsrvsessions = calloc(1, sizeof(void *));
  newsession->peer->udpsrvsessions[0] = newsession;
  newsession->peer->udpsrvsessions_len = 1;
  pthread_mutex_init(&newsession->dtls_mutex, NULL);
  newsession->dtls = NULL;
  return newsession;
}
