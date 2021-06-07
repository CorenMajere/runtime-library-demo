#include <stdio.h>
#include "libfoo.h"
#include "libfoo_nonshared.h"

__attribute__((visibility("hidden")))
const char *foo( void )
{
  puts("TRACE ENTER: foo");
  const char *ret = __libfoo_impl_table_ptr->foo();
  printf("TRACE EXIT: foo: returning \"%s\"\n",ret);
  return ret;
}
