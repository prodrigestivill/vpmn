/***************************************************************************
 *            vpmnd.c
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
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "config.h"
#include "debug.h"
#include "srv.h"
#include "tundev.h"

void
vpmnd_signalhandler (const int sig)
{

  switch (sig)
    {
    case SIGHUP:
      log_info ("Received SIGHUP signal.\n");
      break;
    default:
      log_info ("Unhandled signal %s.\n", sig);
      break;
    }
}

int
vpmnd_start ()
{
  pthread_t tunsrv_thread, udpsrv_thread;
  pid_t pid, sid;

  signal (SIGHUP, vpmnd_signalhandler);
  //signal (SIGTERM, vpmnd_signalhandler);
  //signal (SIGINT, vpmnd_signalhandler);
  //signal (SIGQUIT, vpmnd_signalhandler);

  if (daemonize)
    {
      pid = fork ();
      if (pid < 0)
	return 129;
      /* If we got a good PID, then
         we can exit the parent process. */
      if (pid > 0)
	return 0;

      /* Create a new SID for the child process */
      sid = setsid ();
      if (sid < 0)
	return 130;

      /* Close out the standard file descriptors */
      fclose (stdin);
      fclose (stdout);
      fclose (stderr);
    }

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
  if (!((setgid (vpmnd_gid) == 0) && (setuid (vpmnd_uid) == 0)))
    {
      log_error ("Could not set UID and/or GID to the servers.\n");
      return -1;
    }
  pthread_create (&tunsrv_thread, NULL, (void *) &tunsrv, NULL);
  pthread_create (&udpsrv_thread, NULL, (void *) &udpsrv, NULL);
  config_fistpeersinit ();
  pthread_join (tunsrv_thread, NULL);
  return 0;
}

int
main ()
{
  config_load ();
  return vpmnd_start ();
}
