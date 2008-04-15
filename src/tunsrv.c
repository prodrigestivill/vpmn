/***************************************************************************
 *            tunsrv.c
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "protocol.h"
#include "tundev.h"
#include "config.h"
#include "debug.h"

int tunsrv_pendbufferwait = 0;
int tunsrv_pendbuffer = 0;
pthread_mutex_t tunsrv_pendbuffermutex;
pthread_cond_t tunsrv_mainwaitcond;
pthread_mutex_t tunsrv_mainwaitmutex;
pthread_cond_t tunsrv_threadwaitcond;
pthread_mutex_t tunsrv_threadwaitmutex;

struct tunsrv_buffer_s
{
  pthread_mutex_t buffer_mutex;
  int free;
  char buffer[TUNBUFFERSIZE];
  int buffer_len;
} *tunsrv_buffer;

void tunsrv_thread()
{
  int i;
  while (1)
    {
      pthread_mutex_lock(&tunsrv_threadwaitmutex);
      pthread_cond_wait(&tunsrv_threadwaitcond, &tunsrv_threadwaitmutex);
      pthread_mutex_unlock(&tunsrv_threadwaitmutex);
      for (i = 0; i < num_tunsrvbuffers; i++)
        {
          if (pthread_mutex_trylock(&tunsrv_buffer[i].buffer_mutex) == 0)
            {
              if (tunsrv_buffer[i].free)
                pthread_mutex_unlock(&tunsrv_buffer[i].buffer_mutex);
              else
                {
                  protocol_sendframe(tunsrv_buffer[i].buffer,
                                     tunsrv_buffer[i].buffer_len);
                  tunsrv_buffer[i].free = 1;
                  pthread_mutex_unlock(&tunsrv_buffer[i].buffer_mutex);
                  pthread_mutex_lock(&tunsrv_pendbuffermutex);
                  tunsrv_pendbuffer--;
                  pthread_mutex_unlock(&tunsrv_pendbuffermutex);
                  //Notify main loop about finished job
                  if (tunsrv_pendbufferwait)
                    {
                      pthread_mutex_lock(&tunsrv_mainwaitmutex);
                      pthread_cond_signal(&tunsrv_mainwaitcond);
                      pthread_mutex_unlock(&tunsrv_mainwaitmutex);
                    }
                }
            }
        }
    }
}

void tunsrv()
{
  int rc, i;
  pthread_t tunsrvthreads[num_tunsrvthreads];
  pthread_mutex_init(&tunsrv_mainwaitmutex, NULL);
  pthread_cond_init(&tunsrv_mainwaitcond, NULL);
  pthread_mutex_init(&tunsrv_threadwaitmutex, NULL);
  pthread_cond_init(&tunsrv_threadwaitcond, NULL);
  tunsrv_buffer =
    malloc(num_tunsrvbuffers * sizeof(struct tunsrv_buffer_s));
  for (i = 0; i < num_tunsrvbuffers; i++)
    {
      pthread_mutex_init(&tunsrv_buffer[i].buffer_mutex, NULL);
      tunsrv_buffer[i].free = 1;
    }
  for (i = 0; i < num_tunsrvthreads; i++)
    {
      if ((rc =
           pthread_create(&tunsrvthreads[i], NULL, (void *) &tunsrv_thread,
                          NULL)))
        {
          log_error("Thread %d creation failed: %d\n", i, rc);
          break;
        }
    }

  while (1)
    {
      for (i = 0; i < num_tunsrvbuffers; i++)
        {
          if (pthread_mutex_trylock(&tunsrv_buffer[i].buffer_mutex) == 0)
            {
              if (tunsrv_buffer[i].free)
                {
                  tunsrv_buffer[i].free = 0;
                  tunsrv_buffer[i].buffer_len =
                    tundev_read(tunsrv_buffer[i].buffer, tunmtu);
                  if (tunsrv_buffer[i].buffer_len > 0)
                    {
                      pthread_mutex_unlock(&tunsrv_buffer[i].buffer_mutex);
                      pthread_mutex_lock(&tunsrv_pendbuffermutex);
                      tunsrv_pendbuffer++;
                      pthread_mutex_unlock(&tunsrv_pendbuffermutex);
                      pthread_mutex_lock(&tunsrv_threadwaitmutex);
                      pthread_cond_signal(&tunsrv_threadwaitcond);
                      pthread_mutex_unlock(&tunsrv_threadwaitmutex);
                    }
                  else
                    {
                      tunsrv_buffer[i].free = 1;
                      pthread_mutex_unlock(&tunsrv_buffer[i].buffer_mutex);
                      log_error("Error reading form interface.\n");
                    }
                }
              else
                pthread_mutex_unlock(&tunsrv_buffer[i].buffer_mutex);
            }
        }
      //Wait for free buffer
      if (tunsrv_pendbuffer >= num_tunsrvbuffers)
        {
          tunsrv_pendbufferwait = 1;
          pthread_mutex_lock(&tunsrv_mainwaitmutex);
          pthread_cond_wait(&tunsrv_mainwaitcond, &tunsrv_mainwaitmutex);
          pthread_mutex_unlock(&tunsrv_mainwaitmutex);
          tunsrv_pendbufferwait = 0;
        }
    }
}
