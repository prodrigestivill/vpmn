/***************************************************************************
 *            config.h
 *
 *  Thu Feb 14 18:30:32 2008
 *  Copyright  2008  Pau Rodriguez-Estivill
 *  <prodrigestivill@gmail.com>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <unistd.h>
#include "peer.h"
#include "../config.h"

void config_load ();
void config_fistpeersinit ();

#define DEBUG 1

uid_t vpmnd_uid;
gid_t vpmnd_gid;
int daemonize;

//TUNSRV
#define TUNBUFFERSIZE 1500
char *tunname;
struct in_network tunaddr_ip;
int num_tunsrvthreads;
struct peer_t tun_selfpeer;


//UDPSRV
#define UDPBUFFERSIZE 65535
int num_udpsrvthreads;
int port_udp;
struct udpsrvsession_t **udpsrv_firstpeers;

//CRYPTO
char *ssl_cipherlist;
int ssl_verifydepth;

#endif /* _CONFIG_H */
