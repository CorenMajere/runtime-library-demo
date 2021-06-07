#define _GNU_SOURCE
#include <stdio.h>
#include <link.h>
#include <dlfcn.h>
#include "libfoo_shared.h"

#define TO_STR_HELPER( X ) #X
#define TO_STR( X ) TO_STR_HELPER( X )

#ifndef FOO_IMPL_SO
#error "Must define FOO_IMPL_SO!"
#endif

#ifndef FOO_NAME
#error "Must define FOO_NAME!"
#endif

static __libfoo_impl_list impl = {
  TO_STR(FOO_IMPL_SO), TO_STR(FOO_NAME) " (dlopen)", 0, 0, 0
};

__attribute__((visibility("default")))
void _libfoo_register_impl( __libfoo_impl_list **list )
{
  if ( !list ) return;
  *list = &impl;
  void (*find_reg)( __libfoo_impl_list ** ) = dlsym( RTLD_NEXT, "_libfoo_register_impl" );
  fprintf(stderr,"%s: dlsym( RTLD_NEXT, \"_libfoo_register_impl\" ) = %p\n",__FUNCTION__,find_reg);
  if ( find_reg ) find_reg( &impl.next );
  fprintf(stderr,"%s: list=%p=>{ soname=\"%s\", origin=\"%s\", table=%p, opt_table=%p, next=%p }\n",__FUNCTION__,list,(*list)->soname,(*list)->origin,(*list)->table,(*list)->opt_table,(*list)->next);
}
