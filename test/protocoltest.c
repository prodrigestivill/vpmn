/***************************************************************************
 *            routertest.c
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

struct protocol_1id_s *protocol_v1id;
int protocol_v1id_len;
struct protocol_1_s *protocol_v1ida;
int protocol_v1ida_len;
struct protocol_1ka_s *protocol_v1ka;
int protocol_v1ka_len;
int protocol_v1ka_pos;

void
main ()
{
  int i;
  config_load ();
  protocol_init ();	
  for (i = 0; i < protocol_v1id_len; i++)
    log_debug ("%s.1", ((char *) protocol_v1id + i));
}

void
udpsrvdtls_init ()
{
}

int
udpsrvdtls_loadcerts (const char *cafile, const char *certfile,
		      const char *pkeyfile)
{
  return 0;
}

int
udpsrvdtls_write (const char *buffer, const int buffer_len,
		  struct udpsrvsession_s *session)
{
  return 0;
}

int
tundev_write (const void *buf, const int count)
{
  return 0;
}
