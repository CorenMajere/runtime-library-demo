#include <stdio.h>
#include "libfoo.h"
#include "libfoo_nonshared.h"

__attribute__((visibility("hidden")))
const char *foo_optional( void )
{
  puts("TRACE ENTER: foo_optional");
  const char *ret = __libfoo_opt_impl_table_ptr->foo_optional();
  printf("TRACE EXIT: foo_optional: returning \"%s\"\n",ret);
  return ret;
}

