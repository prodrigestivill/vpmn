#include "../src/tundev.h"

int
main ()
{
  config_load();
  if (tundev_initdev () < 0)
    {
      log_error ("Could not create the interface.\n");
      return -1;
    }
  tundev ();
  return 0;
}
