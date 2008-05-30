/***************************************************************************
 *            ptptest.c
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
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../src/config.h"
#include "../src/debug.h"
#include "../src/srv.h"
#include "../src/udpsrvdtls.h"
#include "../src/udpsrvsession.h"
#include "../src/protocol.h"

int main(int argc, char *argv[])
{
  pthread_t tunsrv_thread, udpsrv_thread;
  struct sockaddr_in *peeraddr;

  if (argc < 10)
    {
      log_error
        ("ptptest dstip tunip tunnm shaip shanm selfip cacert cert key\n");
      return -1;
    }
  //TUNDEV
  tunname = "vpmn0";
  tunmtu = 1500;
  num_tunsrvthreads = 10;
  num_tunsrvbuffers = 50;

  if (inet_aton(argv[2], &tunaddr_ip.addr) == 0
      || inet_aton(argv[3], &tunaddr_ip.netmask) == 0)
    log_error("Unable to load IP configurations.\n");

  tun_selfpeer.shared_networks = calloc(1 + 1, sizeof(struct in_network));
  tun_selfpeer.shared_networks[0].addr.s_addr = tunaddr_ip.addr.s_addr;
  tun_selfpeer.shared_networks[0].netmask.s_addr = 0xffffffff;

  inet_aton(argv[4], &tun_selfpeer.shared_networks[1].addr);
  inet_aton(argv[5], &tun_selfpeer.shared_networks[1].netmask);
  tun_selfpeer.shared_networks[1].addr.s_addr =
    tun_selfpeer.shared_networks[1].addr.s_addr & tun_selfpeer.
    shared_networks[1].netmask.s_addr;
  tun_selfpeer.shared_networks_len = 1 + 1;

  //UDPSRV
  num_udpsrvthreads = 10;
  num_udpsrvbuffers = 50;
  port_udp = 1090;
  tun_selfpeer.addrs = calloc(1, sizeof(struct sockaddr_in));
  inet_aton(argv[6], &tun_selfpeer.addrs[0].sin_addr);
  tun_selfpeer.addrs[0].sin_port = htons(port_udp);
  tun_selfpeer.addrs_len = 1;

  //DTLS
  ssl_verifydepth = 1;
  ssl_cipherlist = "DEFAULT";
  udpsrvdtls_init();
  if (udpsrvdtls_loadcerts(argv[7], argv[8], argv[9]) != 0)
    log_error("Unable to load certs.\n");

  protocol_init();
  //Start program
  if (tundev_initdev() < 0)
    {
      log_error("Could not create the interface.\n");
      return -1;
    }
  if (udpsrv_init() < 0)
    {
      log_error("Could not start the udp server.\n");
      return -1;
    }
  pthread_create(&tunsrv_thread, NULL, (void *) &tunsrv, NULL);
  pthread_create(&udpsrv_thread, NULL, (void *) &udpsrv, NULL);
  //Start fist connection
  if (argv[1][0] != '-')
    {
      peeraddr = malloc(sizeof(struct sockaddr_in));
      peeraddr->sin_port = htons(1090);
      peeraddr->sin_family = AF_INET;
      inet_aton(argv[1], &peeraddr->sin_addr);
      udpsrv_firstpeers = calloc(1, sizeof(struct udpsrvsession_s *));
      udpsrv_firstpeers[0] = udpsrvsession_searchcreate(peeraddr);
      if (udpsrv_firstpeers[0]->peer == NULL)
        udpsrv_firstpeers[0]->peer = peer_create();
      protocol_sendpacket(udpsrv_firstpeers[0], PROTOCOL1_ID);
    }
  //Program ended
  pthread_join(tunsrv_thread, NULL);
}
