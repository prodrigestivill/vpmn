#include "../src/tundev.h"

int
main ()
{
  config_load();
  tundev ();
  return 0;
}
