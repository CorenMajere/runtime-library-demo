#include "libfoo.h"

__attribute__((visibility("hidden")))
const char *foo( void )
{
   return "Implementation 1";
}
