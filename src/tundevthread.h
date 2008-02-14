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

#ifndef _TUNDEVTHREAD_H
#define _TUNDEVTHREAD_H

#include <pthread.h>

struct tundevthread_t
{
  pthread_t thread;
  pthread_mutex_t thread_mutex;
  pthread_cond_t cond;
  pthread_mutex_t cond_mutex;
  char buffer[65535 * sizeof (char)];
  int buffer_len;
};

void tundevthread (struct tundevthread_t *me);
int tundevthread_create (struct tundevthread_t *new);
#endif /* _TUNDEVTHREAD_H */
