/***************************************************************************
 *            config.c
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

#include "debug.h"
#include "config.h"
#include "udpsrvdtls.h"
#include "protocol.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void
config_load ()
{
  vpmnd_uid = 1000;
  vpmnd_gid = 1000;
#ifdef DEBUG
  daemonize = 0;
#else
  daemonize = 1;
#endif
  //TUNDEV
  tunname = "vpmn0";
  num_tunsrvthreads = 5;
	
  char *tunaddr_ip_str = "10.0.0.5";
  char *tunaddr_nm_str = "255.255.255.0";
  if (inet_aton (tunaddr_ip_str, &tunaddr_ip.addr) == 0
      || inet_aton (tunaddr_nm_str, &tunaddr_ip.netmask) == 0)
    log_error ("Unable to load IP configurations.\n");

  tun_selfpeer.shared_networks = calloc (1+1, sizeof (struct in_network));
  tun_selfpeer.shared_networks[0].addr.s_addr = tunaddr_ip.addr.s_addr;
  tun_selfpeer.shared_networks[0].netmask.s_addr = 0xffffffff;
	
  char *tunaddr_net0_ip_str = "10.1.0.5";
  char *tunaddr_net0_nm_str = "255.255.255.0";
  inet_aton (tunaddr_net0_ip_str, &tun_selfpeer.shared_networks[1].addr);
  inet_aton (tunaddr_net0_nm_str, &tun_selfpeer.shared_networks[1].netmask);
  tun_selfpeer.shared_networks[1].addr.s_addr = tun_selfpeer.shared_networks[1].addr.s_addr &
		tun_selfpeer.shared_networks[1].netmask.s_addr;
  tun_selfpeer.shared_networks_len = 1+1;

  //UDPSRV
  num_udpsrvthreads = 5;
  port_udp = 1090;

  //DTLS
  char *sslcacert_str = "../test/cacert.pem";
  char *sslcert_str = "../test/client1cert.pem";
  char *sslpkey_str = "../test/client1pkey.pem";
  ssl_verifydepth = 1;
  ssl_cipherlist = "DEFAULT";
  udpsrvdtls_init ();
  if (udpsrvdtls_loadcerts (sslcacert_str, sslcert_str, sslpkey_str) != 0)
    log_error ("Unable to load certs.\n");
}

void
config_fistpeersinit ()
{
  struct sockaddr_in *peeraddr;
  char *peeraddr_str;
  peeraddr = malloc (sizeof (struct sockaddr_in));
  peeraddr_str = "127.0.0.1";
  peeraddr->sin_port = htons (1091);
  peeraddr->sin_family = AF_INET;
  inet_aton (peeraddr_str, &peeraddr->sin_addr);
  udpsrv_firstpeers = calloc (1, sizeof (struct udpsrvsession_t *));
  udpsrv_firstpeers[0] = udpsrvsession_search (peeraddr);
  protocol_sendpacket (udpsrv_firstpeers[0]->peer, PROTOCOL1_ID);
}
