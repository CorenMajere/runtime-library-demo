#include "libfoo.h"
#include "libfoo_nonshared.h"

__attribute__((visibility("hidden")))
const char *foo( void )
{
  return __libfoo_impl_table_ptr->foo();
}
