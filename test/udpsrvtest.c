#include "../src/config.h"
void udpsrv ();

int
main ()
{
  config_load();
  udpsrv ();
  return 0;
}
