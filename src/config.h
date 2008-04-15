/***************************************************************************
 *            config.h
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

#ifndef _CONFIG_H
#define _CONFIG_H

#include <netinet/ip.h>
#include <unistd.h>
#include "peer.h"
#include "../config.h"

void config_load(char *cfgfile);
void config_fistpeersinit();

#define DEBUG 1

uid_t vpmnd_uid;
gid_t vpmnd_gid;
int daemonize;

//PROTOCOL
#define MAXPEERS 512

//TUNSRV
#define TUNBUFFERSIZE 1500
char *tunname;
int tunmtu;
struct in_network tunaddr_ip;
int num_tunsrvthreads;
int num_tunsrvbuffers;
struct peer_s tun_selfpeer;


//UDPSRV
#define UDPMTUSIZE 1500
#define UDPBUFFERSIZE 65535     //UDPMTUSIZE - 160 // IPv4
int num_udpsrvthreads;
int num_udpsrvbuffers;
int port_udp;
struct udpsrvsession_s **udpsrv_firstpeers;

//CRYPTO
char *ssl_cipherlist;
int ssl_verifydepth;

#endif                          /* _CONFIG_H */
