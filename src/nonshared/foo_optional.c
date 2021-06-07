#include "libfoo.h"
#include "libfoo_nonshared.h"

__attribute__((visibility("hidden")))
const char *foo_optional( void )
{
  return __libfoo_opt_impl_table_ptr->foo_optional();
}

