/***************************************************************************
 *            udpsrvsessiontest.c
 *
 *  Tue Feb  6 23:58:12 2008
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

#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <strings.h>

#include <netinet/in.h>
#include <arpa/inet.h>


#include "../src/debug.h"
#include "../src/config.h"
#include "../src/udpsrvsession.h"

int
main ()
{
  config_load ();
  char buffer[UDPBUFFERSIZE];
  int buffer_len;
  char *s_addr;
  int s_port;
  struct udpsrvsession_t *udpsession;
  struct sockaddr_in addr;
  socklen_t addr_len;

  log_debug ("Starting...\n");
  int sd_udp;
  struct sockaddr_in bind_addr;
  bzero (&bind_addr, sizeof (bind_addr));
  sd_udp = socket (PF_INET, SOCK_DGRAM, 0);
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  bind_addr.sin_port = htons (port_udp);
  if (bind (sd_udp, (struct sockaddr *) &bind_addr, sizeof (bind_addr)) != 0)
    log_error ("Bind error\n");
  log_debug ("Listening...\n");
  while (1)
    {
      addr_len = sizeof (addr);
      bzero (&addr, addr_len);
      buffer_len =
	recvfrom (sd_udp, buffer, sizeof (buffer), 0,
		  (struct sockaddr *) &(addr), &(addr_len));
      s_addr = inet_ntoa (addr.sin_addr);
      s_port = ntohs (addr.sin_port);
      udpsession = udpsrvsession_search (&addr);
      //log_debug ("Main  : %s:%d %d \"%s\"\n", s_addr, s_port, udpsession->fd,
      // buffer);
      log_debug ("Main  : %d \n", udpsession->dtls);
    }
}
