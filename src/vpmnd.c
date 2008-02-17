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

#include <pthread.h>
#include "config.h"
#include "debug.h"
#include "srv.h"
#include "tundev.h"

int
main ()
{
  pthread_t tunsrv_thread, udpsrv_thread;
  config_load ();
  if (tundev_initdev () < 0)
    {
      log_error ("Could not create the interface.\n");
      return -1;
    }
  if (udpsrv_init () < 0)
    {
      log_error ("Could not start the udp server.\n");
      return -1;
    }
  //Change UID
  pthread_create (&tunsrv_thread, NULL, (void *) &tunsrv, NULL);
  pthread_create (&udpsrv_thread, NULL, (void *) &udpsrv, NULL);
  return 0;
}
