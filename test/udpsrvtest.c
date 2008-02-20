#include "../src/debug.h"
#include "../src/config.h"
#include "../src/srv.h"

int
main ()
{
  config_load ();
  if (udpsrv_init () < 0)
    {
      log_error ("Could not create the interface.\n");
      return -1;
    }
  udpsrv ();
  return 0;
}

void
protocol_recvpacket (const char *tunbuffer, const int tunbuffer_len,
		     struct peer_t *peer)
{
  log_debug("Recv: %s", tunbuffer);
}

void
protocol_sendframe (const char *buffer, const int buffer_len)
{
	log_debug("Sending frame... (not implemented)");
}

void
protocol_sendroutes (const struct peer_t *dstpeer)
{
  log_debug("Sending routes... (not implemented)");
}
