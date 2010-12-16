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
#include "udpsrvsession.h"
#include "udpsrvdtls.h"
#include "protocol.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CONFIG_READ_LINE 1024

int config_istrue(const char *val)
{
	if (*val=='T' || *val=='t')
		return 1;
	if (*val=='0' || *val=='F' || *val=='f')
		return 0;
	return atoi(val);
}

void config_load(char *cfgfile)
{
	char *sslcacert_str = NULL, *sslcert_str = NULL, *sslpkey_str = NULL;
	char buf[CONFIG_READ_LINE], *key, *val, *end;
	FILE *fd = fopen(cfgfile, "r");
	while(fgets(buf, CONFIG_READ_LINE, fd) != NULL){
		if (*buf == '#' || *buf == '/')
			continue;
		for(key = buf;*key==' ';key++);
		end = strchr(buf, '=');
		if (end == NULL)
			continue;
		for(val=end+1;*val==' ';val++);
		*end = '\0';
		for(end--;*end==' ';end--)
			*end = '\0';
		for(end = &val[strlen(val)-1];
					*end==' ' ||
					*end=='\r' ||
					*end=='\n'; end--)
			*end = '\0';
		//Keys
		if (strcmp(key, "uid") == 0){
			vpmnd_uid = atoi(val);
		}else
		if (strcmp(key, "gid") == 0){
			vpmnd_gid = atoi(val);
		}else
		if (strcmp(key, "daemonize") == 0){
#ifdef DEBUG
			daemonize = 0;
#else
  		daemonize = config_istrue(val);
#endif
		}else
		if (strcmp(key, "tunname") == 0){
			tunname = strdup(val);
		}else
		if (strcmp(key, "tunmtu") == 0){
			tunmtu = atoi(val);
		}else
		if (strcmp(key, "num_tunsrvthreads") == 0){
			num_tunsrvthreads = atoi(val);
		}else
		if (strcmp(key, "num_tunsrvbuffers") == 0){
			num_tunsrvbuffers = atoi(val);
		}else
		if (strcmp(key, "tunaddr_ip") == 0){
			if (inet_aton(val, &tunaddr_ip.addr) == 0){
				log_error("Unable to load IP configuration.\n");
			}else{
			tun_selfpeer.shared_networks = calloc(1, sizeof(struct in_network));
			tun_selfpeer.shared_networks[0].addr.s_addr = tunaddr_ip.addr.s_addr;
			tun_selfpeer.shared_networks[0].netmask.s_addr = 0xffffffff;
			tun_selfpeer.shared_networks_len = 1;
			}
		}else
		if (strcmp(key, "tunaddr_nm") == 0){
			if (inet_aton(val, &tunaddr_ip.netmask) == 0)
				log_error("Unable to load NM configuration.\n");
		}else
/*
char *tunaddr_net0_ip_str = "10.1.0.5";
char *tunaddr_net0_nm_str = "255.255.255.0";
inet_aton(tunaddr_net0_ip_str, &tun_selfpeer.shared_networks[1].addr);
inet_aton(tunaddr_net0_nm_str, &tun_selfpeer.shared_networks[1].netmask);
tun_selfpeer.shared_networks[1].addr.s_addr =
  tun_selfpeer.shared_networks[1].addr.s_addr & tun_selfpeer.
  shared_networks[1].netmask.s_addr;
tun_selfpeer.shared_networks_len++;
*/
		if (strcmp(key, "num_udpsrvthreads") == 0){
			num_udpsrvthreads = atoi(val);
		}else
		if (strcmp(key, "num_udpsrvbuffers") == 0){
			num_udpsrvbuffers = atoi(val);
		}else		
		if (strcmp(key, "port_udp") == 0){
			port_udp = atoi(val);
		}else		
/*
char *updsrv_ip_str = "192.168.0.2";
tun_selfpeer.addrs = calloc(1, sizeof(struct sockaddr_in));
inet_aton(updsrv_ip_str, &tun_selfpeer.addrs[0].sin_addr);
tun_selfpeer.addrs[0].sin_port = htons(port_udp);
tun_selfpeer.addrs_len = 1;
*/
		if (strcmp(key, "sslcacert") == 0){
			sslcacert_str = strdup(val);
		}else
		if (strcmp(key, "sslcert") == 0){
			sslcert_str = strdup(val);
		}else
		if (strcmp(key, "sslpkey") == 0){
			sslpkey_str = strdup(val);
		}else
		if (strcmp(key, "ssl_verifydepth") == 0){
			ssl_verifydepth = atoi(val);
		}else
		if (strcmp(key, "sslpkey") == 0){
			ssl_cipherlist = strdup(val);
		}
	}
	udpsrvdtls_init();
	if (sslcacert_str!=NULL && sslcert_str!=NULL && sslpkey_str!=NULL &&
		 (udpsrvdtls_loadcerts(sslcacert_str, sslcert_str, sslpkey_str) != 0)){
		log_error("Unable to load certs.\n");
	}
	if (sslcacert_str!=NULL)
		free(sslcacert_str);
	if (sslcert_str!=NULL)
		free(sslcert_str);
	if (sslpkey_str!=NULL)		
		free(sslpkey_str);
}

void config_fistpeersinit()
{
/*
  struct sockaddr_in *peeraddr;
  char *peeraddr_str;
  peeraddr = malloc (sizeof (struct sockaddr_in));
  peeraddr_str = "127.0.0.1";
  peeraddr->sin_port = htons (1091);
  peeraddr->sin_family = AF_INET;
  inet_aton (peeraddr_str, &peeraddr->sin_addr);
  udpsrv_firstpeers = calloc (1, sizeof (udpsrvsession_s *));
  udpsrv_firstpeers[0] = udpsrvsession_search (peeraddr);
  protocol_sendpacket (udpsrv_firstpeers[0]->peer, PROTOCOL1_ID);
*/
}
