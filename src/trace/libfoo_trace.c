#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include "libfoo.h"
#include "libfoo_shared.h"
#include "libfoo_trace.h"

static const char *init( const char *origin )
{
  printf("TRACE ENTER: Initialization: origin=\"%s\"\n",origin);
  const char *ret = __libfoo_impl_table_ptr->init(origin);
  printf("TRACE EXIT: Initialization: returning \"%s\"\n",ret);
  return ret;
}

static void opt_init( void )
{
  puts("TRACE ENTER: Optional Initialization");
  __libfoo_opt_impl_table_ptr->init();
  puts("TRACE EXIT: Optional Initialization");
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

__libfoo_impl_table *__libfoo_impl_table_ptr = 0;
__libfoo_opt_impl_table *__libfoo_opt_impl_table_ptr = 0;

static void get_tables( void )
{
  __libfoo_impl_table *(*get_table)( void ) = dlsym( RTLD_NEXT, "_libfoo_get_impl_table" );
  __libfoo_opt_impl_table *(*get_opt_table)( void ) = dlsym( RTLD_NEXT, "_libfoo_get_opt_impl_table" );

  if ( get_table && get_opt_table ) {
    __libfoo_impl_table_ptr = get_table();
    __libfoo_opt_impl_table_ptr = get_opt_table();
  }
  else {
    fprintf(stderr,"ERROR: Table lookup failure (%p, %p)!\n",get_table,get_opt_table);
    abort();
  }
}

__attribute__((visibility("default")))
__libfoo_impl_table *_libfoo_get_impl_table( void )
{
  if ( !__libfoo_impl_table_ptr ) get_tables();
  return &table;
}

__attribute__((visibility("default")))
__libfoo_opt_impl_table *_libfoo_get_opt_impl_table( void )
{
  if ( !__libfoo_opt_impl_table_ptr ) get_tables();
  return &opt_table;
}

