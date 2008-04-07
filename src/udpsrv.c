/***************************************************************************
 *            udpsrv.c
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
#include <strings.h>
#include <resolv.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "udpsrvdtls.h"
#include "udpsrvsession.h"
#include "protocol.h"
#include "debug.h"
#include "config.h"
#include "srv.h"

int udpsrv_fd = -1;
int udpsrv_pendbufferwait = 0;
int udpsrv_pendbuffer = 0;
pthread_mutex_t udpsrv_pendbuffermutex;
pthread_cond_t udpsrv_mainwaitcond;
pthread_mutex_t udpsrv_mainwaitmutex;
pthread_cond_t udpsrv_threadwaitcond;
pthread_mutex_t udpsrv_threadwaitmutex;

struct udpsrv_buffer_s
{
  pthread_mutex_t buffer_mutex;
  int free;
  char buffer[UDPBUFFERSIZE];
  int buffer_len;
  struct sockaddr_in addr;
  socklen_t addr_len;
} *udpsrv_buffer;

int udpsrv_init()
{
  struct sockaddr_in bind_addr;
  bzero(&bind_addr, sizeof(bind_addr));
  udpsrv_fd = socket(PF_INET, SOCK_DGRAM, 0);
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  bind_addr.sin_port = htons(port_udp);
  if (bind(udpsrv_fd, (struct sockaddr *) &bind_addr, sizeof(bind_addr)) !=
      0)
    {
      log_error("Bind error\n");
      return -1;
    }
  return 0;
}

void udpsrv_thread()
{
  char tunbuffer[TUNBUFFERSIZE];
  int i, tunbuffer_len;
  struct udpsrvsession_s *udpsession = NULL;
  while (1)
    {
      pthread_mutex_lock(&udpsrv_threadwaitmutex);
      pthread_cond_wait(&udpsrv_threadwaitcond, &udpsrv_threadwaitmutex);
      pthread_mutex_unlock(&udpsrv_threadwaitmutex);
      for (i = 0; i < num_udpsrvbuffers; i++)
        {
          if (pthread_mutex_trylock(&udpsrv_buffer[i].buffer_mutex) == 0)
            {
              if (udpsrv_buffer[i].free)
                pthread_mutex_unlock(&udpsrv_buffer[i].buffer_mutex);
              else
                {
                  udpsession =
                    udpsrvsession_searchcreate(&udpsrv_buffer[i].addr);
                  tunbuffer_len =
                    udpsrvdtls_read(udpsrv_buffer[i].buffer,
                                    udpsrv_buffer[i].buffer_len, tunbuffer,
                                    TUNBUFFERSIZE, udpsession);
                  if (tunbuffer_len > 0)
                    protocol_recvpacket(tunbuffer, tunbuffer_len,
                                        udpsession);
                  udpsrv_buffer[i].free = 1;
                  pthread_mutex_unlock(&udpsrv_buffer[i].buffer_mutex);
                  pthread_mutex_lock(&udpsrv_pendbuffermutex);
                  udpsrv_pendbuffer--;
                  pthread_mutex_unlock(&udpsrv_pendbuffermutex);
                  //Notify main loop about finished job
                  if (udpsrv_pendbufferwait)
                    {
                      pthread_mutex_lock(&udpsrv_mainwaitmutex);
                      pthread_cond_signal(&udpsrv_mainwaitcond);
                      pthread_mutex_unlock(&udpsrv_mainwaitmutex);
                    }
                }
            }
        }
    }
}

void udpsrv()
{
  int rc, i;
  pthread_t udpsrvthreads[num_udpsrvthreads];
  pthread_mutex_init(&udpsrv_mainwaitmutex, NULL);
  pthread_cond_init(&udpsrv_mainwaitcond, NULL);
  pthread_mutex_init(&udpsrv_threadwaitmutex, NULL);
  pthread_cond_init(&udpsrv_threadwaitcond, NULL);
  udpsrv_buffer =
    malloc(num_udpsrvbuffers * sizeof(struct udpsrv_buffer_s));
  for (i = 0; i < num_udpsrvbuffers; i++)
    {
      pthread_mutex_init(&udpsrv_buffer[i].buffer_mutex, NULL);
      udpsrv_buffer[i].free = 1;
    }
  for (i = 0; i < num_udpsrvthreads; i++)
    {
      if ((rc =
           pthread_create(&udpsrvthreads[i], NULL, (void *) &udpsrv_thread,
                          NULL)))
        {
          log_error("Thread %d creation failed: %d\n", i, rc);
          break;
        }
    }

  while (1)
    {
      for (i = 0; i < num_udpsrvbuffers; i++)
        {
          if (pthread_mutex_trylock(&udpsrv_buffer[i].buffer_mutex) == 0)
            {
              if (udpsrv_buffer[i].free)
                {
                  udpsrv_buffer[i].free = 0;
                  udpsrv_buffer[i].addr_len =
                    sizeof(udpsrv_buffer[i].addr);
                  bzero(&udpsrv_buffer[i].addr, udpsrv_buffer[i].addr_len);
                  udpsrv_buffer[i].buffer_len =
                    recvfrom(udpsrv_fd, udpsrv_buffer[i].buffer,
                             sizeof(udpsrv_buffer[i].buffer), 0,
                             (struct sockaddr *) &udpsrv_buffer[i].addr,
                             &udpsrv_buffer[i].addr_len);
                  if (udpsrv_buffer[i].buffer_len > 0)
                    {
                      pthread_mutex_unlock(&udpsrv_buffer[i].buffer_mutex);
                      pthread_mutex_lock(&udpsrv_pendbuffermutex);
                      udpsrv_pendbuffer++;
                      pthread_mutex_unlock(&udpsrv_pendbuffermutex);
                      pthread_mutex_lock(&udpsrv_threadwaitmutex);
                      pthread_cond_signal(&udpsrv_threadwaitcond);
                      pthread_mutex_unlock(&udpsrv_threadwaitmutex);
                    }
                  else
                    {
                      udpsrv_buffer[i].free = 1;
                      pthread_mutex_unlock(&udpsrv_buffer[i].buffer_mutex);
                      log_error("Error reading form socket.\n");
                    }
                }
              else
                pthread_mutex_unlock(&udpsrv_buffer[i].buffer_mutex);
            }
        }
      //Wait for free buffer
      if (udpsrv_pendbuffer >= num_udpsrvbuffers)
        {
          udpsrv_pendbufferwait = 1;
          pthread_mutex_lock(&udpsrv_mainwaitmutex);
          pthread_cond_wait(&udpsrv_mainwaitcond, &udpsrv_mainwaitmutex);
          pthread_mutex_unlock(&udpsrv_mainwaitmutex);
          udpsrv_pendbufferwait = 0;
        }
    }
}
