#include <stdio.h>
#include "libfoo.h"
#include "libfoo_shared.h"

static const char *init( const char *origin )
{
  if ( foo_version_string ) return foo_version_string;
  printf("Implementation 1 initialized from \"%s\"!\n",origin);
  return foo_version_string = "Implementation 1";
}

static void opt_init( void )
{
  static int called = 0;
  if ( called ) return;
  called = 1;
  puts("Implementation 1 optional functionality initialized!");
}

static __libfoo_impl_table table = {
  { LIBFOO_SHARED_MAJOR, LIBFOO_SHARED_MINOR },
  init,
  foo
};

static __libfoo_opt_impl_table opt_table = {
  { LIBFOO_SHARED_OPT_MAJOR, LIBFOO_SHARED_OPT_MINOR },
  opt_init,
  foo_optional
};

__attribute__((visibility("default")))
__libfoo_impl_table *_libfoo_get_impl_table( void )
{
  return &table;
}

__attribute__((visibility("default")))
__libfoo_opt_impl_table *_libfoo_get_opt_impl_table( void )
{
  return &opt_table;
}

