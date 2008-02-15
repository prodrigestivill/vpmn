#include "../src/config.h"
int udpsrv_init ();
void udpsrv ();

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
