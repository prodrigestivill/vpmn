/***************************************************************************
 *            udpsrvtest.c
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

#include "../src/debug.h"
#include "../src/config.h"
#include "../src/srv.h"

int main()
{
  config_load("");
  if (udpsrv_init() < 0)
    {
      log_error("Could not create the interface.\n");
      return -1;
    }
  udpsrv();
  return 0;
}

void
protocol_recvpacket(const char *tunbuffer, const int tunbuffer_len,
                    void *session, const int sessiontype)
{
  log_debug("Recv: %s", tunbuffer);
}

void protocol_sendframe(const char *buffer, const int buffer_len)
{
  log_debug("Sending frame... (not implemented)");
}

void protocol_sendroutes(const struct peer_s *dstpeer)
{
  log_debug("Sending routes... (not implemented)");
}
