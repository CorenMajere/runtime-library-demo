#include "libfoo.h"

__attribute__((visibility("hidden")))
const char *foo_optional( void )
{
  return "Implementation 2";
}

