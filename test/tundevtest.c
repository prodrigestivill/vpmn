#include "../src/config.h"
#include "../src/debug.h"
#include "../src/tundev.h"
void tunsrv ();

int
main ()
{
  config_load();
  if (tundev_initdev () < 0)
    {
      log_error ("Could not create the interface.\n");
      return -1;
    }
  tunsrv ();
  return 0;
}
