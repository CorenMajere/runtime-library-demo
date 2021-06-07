#include <stdlib.h>
#include <stdio.h>
#include "libfoo_shared.h"
#include "libfoo_nonshared.h"

static void get_impl_table_ptr( void );
static void uninit_abort( void );

static __libfoo_impl_table uninitialized_table = {
  { LIBFOO_SHARED_MAJOR, LIBFOO_SHARED_MINOR },
  (__libfoo_init_t) get_impl_table_ptr,
  (const char *(*)( void )) uninit_abort
};

__libfoo_impl_table *__libfoo_impl_table_ptr = &uninitialized_table;

__attribute__((constructor))
static void get_impl_table_ptr( void )
{
  if ( __libfoo_impl_table_ptr != &uninitialized_table ) return;
  __libfoo_impl_table_ptr = _libfoo_get_impl_table();
  if ( __libfoo_impl_table_ptr->version.major != LIBFOO_SHARED_MAJOR ||
       __libfoo_impl_table_ptr->version.minor <  LIBFOO_SHARED_MINOR ) {
    fprintf(stderr,"ERROR: __libfoo_impl_table version %d.%d incompatible with expected version %d.%d!\n",__libfoo_impl_table_ptr->version.major,__libfoo_impl_table_ptr->version.minor,LIBFOO_SHARED_MAJOR,LIBFOO_SHARED_MINOR);
    abort();
  }
  __libfoo_impl_table_ptr->init("Direct");
}

static void uninit_abort( void )
{
  fprintf(stderr,"ERROR: __libfoo_impl_table not initialized before first call!\n");
  abort();
}
