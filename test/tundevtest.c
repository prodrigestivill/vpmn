#include "../src/config.h"
#include "../src/debug.h"
#include "../src/tundev.h"
#include "../src/udpsrvsession.h"
#include "../src/srv.h"

int
main ()
{
  config_load ();
  if (tundev_initdev () < 0)
    {
      log_error ("Could not create the interface.\n");
      return -1;
    }
  tunsrv ();
  return 0;
}

void
protocol_sendframe (const char *buffer, const int buffer_len)
{
  struct in_addr src4, dst4;
  //Check for IPv4
  if ((buffer[0] & 0xF0) == 0x40)
    {
      src4.s_addr = (buffer[12]) | (buffer[13] << 8) |
	(buffer[14] << 16) | (buffer[15] << 24);
      dst4.s_addr = (buffer[16]) | (buffer[17] << 8) |
	(buffer[18] << 16) | (buffer[19] << 24);
      if (router_checksrc (&src4) == 0)
	{
	  log_debug ("%s", inet_ntoa (src4));
	  log_debug ("->%s\n", inet_ntoa (dst4));
	}
      else
	log_error ("Invalid source.\n");
    }
  //Check for IPv6
  else if ((buffer[0] & 0xF0) == 0x60)

    {
      log_error ("IPv6 not implemented.\n");
    }
  else
    log_error ("Unknow protocol not implemented.\n");
}

void
protocol_recvpacket (const char *tunbuffer, const int tunbuffer_len,
		     struct peer_t *peer)
{
  log_debug ("Recive packet... (not implemented)");
}

void
protocol_sendroutes (const struct peer_t *dstpeer)
{
  log_debug ("Sending routes... (not implemented)");
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

struct udpsrvsession_t *
udpsrvsession_search (struct sockaddr_in *source)
{
  struct udpsrvsession_t *newsession =
    malloc (sizeof (struct udpsrvsession_t));
  newsession->addr = source;
  newsession->peer = peer_create ();
  newsession->peer->udpsrvsession = newsession;
  pthread_mutex_init (&newsession->dtls_mutex, NULL);
  newsession->dtls = NULL;
  return newsession;
}
