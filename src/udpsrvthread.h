/***************************************************************************
 *            udpsrvthread.h
 *
 *  Tue Feb  5 11:13:32 2008
 *  Copyright  2008  Pau Rodriguez-Estivill
 *  <prodrigestivill@gmail.com>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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

#ifndef _UDPSRVTHREAD_H
#define _UDPSRVTHREAD_H

#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>

struct udpsrvthread_t
{
  pthread_t thread;
  pthread_cond_t cond;
  pthread_mutex_t cond_mutex;
  char *buffer;
  int buffer_len;
  struct sockaddr_in addr;
  socklen_t addr_len;
};

void udpsrvthread (struct udpsrvthread_t *me);

#endif /* _UDPSRVTHREAD_H */
