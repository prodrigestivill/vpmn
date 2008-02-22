/***************************************************************************
 *            udpsrvsessiontest.c
 *
 *  Copyright  2008  Pau Rodriguez-Estivill
 *  <prodrigestivill@gmail.com>
 ****************************************************************************/

/* This program is free software: you can redistribute it and/or modify
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

#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <strings.h>

#include <netinet/in.h>
#include <arpa/inet.h>


#include "../src/debug.h"
#include "../src/config.h"
#include "../src/udpsrvsession.h"
#include "../src/udpsrvdtls.h"
#include "../src/srv.h"

int
main ()
{
  config_load ();
  char buffer[UDPBUFFERSIZE], bufferout[TUNBUFFERSIZE];
  int buffer_len, bufferout_len;
  struct udpsrvsession_t *udpsession;
  struct sockaddr_in addr;
  struct sockaddr_in *addr2;
  socklen_t addr_len;
  log_debug ("Starting...\n");
  struct sockaddr_in bind_addr;
  bzero (&bind_addr, sizeof (bind_addr));
  udpsrv_fd = socket (PF_INET, SOCK_DGRAM, 0);
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  bind_addr.sin_port = htons (port_udp);
  if (bind (udpsrv_fd, (struct sockaddr *) &bind_addr, sizeof (bind_addr)) != 0)
    log_error ("Bind error\n");
  log_debug ("Listening...\n");
  while (1)
    {
      addr_len = sizeof (addr);
      bzero (&addr, addr_len);
      buffer_len =
	recvfrom (udpsrv_fd, buffer, sizeof (buffer), 0,
		  (struct sockaddr *) &(addr), &(addr_len));
      addr2 = malloc (sizeof (struct sockaddr_in));
      bcopy (&addr, addr2, addr_len);
      udpsession = udpsrvsession_search (addr2);
      bufferout_len =
	udpsrvdtls_read (buffer, buffer_len, bufferout, TUNBUFFERSIZE,
			 udpsession);
      if (bufferout_len > 0)
	log_debug ("Decoded: %s \n", bufferout);
      else
	log_error ("No decoded data.\n");
    }
}

void
protocol_sendroutes (const struct peer_t *dstpeer)
{
  log_debug ("Sending routes... (not implemented)");
}

