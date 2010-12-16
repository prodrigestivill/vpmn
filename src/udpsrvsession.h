/***************************************************************************
 *            udpsrvsession.h
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

#ifndef _UDPSRVSESSION_H
#define _UDPSRVSESSION_H

#include <pthread.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include "timeout.h"
#include "router.h"
#include "peer.h"

struct udpsrvsession_s
{
  timeout_t timeout;
  SSL *dtls;
  BIO *bioread;
  pthread_mutex_t dtls_mutex;
  pthread_mutex_t bioread_mutex;
  int dtls_reading;
  const struct sockaddr_in *addr;
  struct peer_s *peer;
};

struct udpsrvsession_s *udpsrvsession_search(const struct sockaddr_in
                                             *source);
struct udpsrvsession_s *udpsrvsession_searchcreate(const struct sockaddr_in
                                                   *source);
void udpsrvsession_destroy(struct udpsrvsession_s *cursession);
int udpsrvsession_dumpsocks(void *out, const int outlen, const int start,
                            const int num);

#endif                          /* _UDPSRVSESSION_H */
