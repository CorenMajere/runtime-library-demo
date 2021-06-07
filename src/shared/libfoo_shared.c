#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include "libfoo.h"
#include "libfoo_shared.h"

const char *__foo_version_string __attribute__((visibility("hidden")));
const char *__foo_version_string = 0;
extern const char *foo_version_string __attribute__((weak,alias("__foo_version_string")));

static const char *init( const char *origin )
{
  if ( foo_version_string ) return foo_version_string;
  printf("Default implementation initialized from \"%s\"!\n",origin);
  return foo_version_string = "Default";
}

static void opt_init( void )
{
  static int called = 0;
  if ( called ) return;
  called = 1;
  puts("Default implementation optional functionality initialized!");
}

static __libfoo_impl_table default_table = {
  { LIBFOO_SHARED_MAJOR, LIBFOO_SHARED_MINOR },
  init,
  foo
};

static __libfoo_opt_impl_table default_opt_table = {
  { LIBFOO_SHARED_OPT_MAJOR, LIBFOO_SHARED_OPT_MINOR },
  opt_init,
  foo_optional
};

static __libfoo_impl_list impl_list_env = {
  0, "Environment (LIBFOO_IMPL)", 0, 0, 0
};

static __libfoo_impl_list impl_list_default = {
  0, "Default", &default_table, &default_opt_table, 0
};

static __libfoo_impl_list impl_list_linked = {
  0, "Linked", 0, 0, &impl_list_default
};

static __libfoo_impl_table *table = 0;
static __libfoo_opt_impl_table *opt_table = 0;

static void get_impl_list( __libfoo_impl_list **impl_list )
{
  if ( !impl_list ) return;

 impl_list_env.soname = getenv("LIBFOO_IMPL");
 *impl_list = &impl_list_env;

  void (*find_reg)( __libfoo_impl_list ** ) = dlsym( RTLD_DEFAULT, "_libfoo_register_impl" );
  fprintf(stderr,"%s: dlsym( RTLD_DEFAULT, \"_libfoo_register_impl\" ) = %p\n",__FUNCTION__,find_reg);
  if ( find_reg ) {
    find_reg( &impl_list_env.next );
    fprintf(stderr,"%s: _libfoo_register_impl( &impl_list_env.next ) = %p\n",__FUNCTION__,impl_list_env.next);
  }

  __libfoo_impl_list *li = &impl_list_env;
  while ( li->next != 0 ) li = li->next;

  __libfoo_impl_table *(*get_table)( void ) = dlsym( RTLD_NEXT, "_libfoo_get_impl_table" );
  __libfoo_opt_impl_table *(*get_opt_table)( void ) = dlsym( RTLD_NEXT, "_libfoo_get_opt_impl_table" );

  if ( get_table && get_opt_table ) {
    impl_list_linked.table = get_table();
    impl_list_linked.opt_table = get_opt_table();
    li->next = &impl_list_linked;
  }
  else {
    li->next = &impl_list_default;
  }
}

static void get_tables( void )
{
  __libfoo_impl_table *(*get_table)( void );
  __libfoo_opt_impl_table *(*get_opt_table)( void );

  __libfoo_impl_list *li;
  get_impl_list( &li );
  while ( li ) {
    if ( li->table && li->opt_table ) {
      break;
    }
    else if ( li->soname ) {
      void *h = dlopen( li->soname, RTLD_LAZY|RTLD_NOLOAD );
      fprintf(stderr,"%s: dlopen( \"%s\", RTLD_LAZY|RTLD_NOLOAD ) = %p\n",__FUNCTION__,li->soname,h);
      if ( !h ) {
        h = dlopen( li->soname, RTLD_LAZY );
        fprintf(stderr,"%s: dlopen( \"%s\", RTLD_LAZY ) = %p\n",__FUNCTION__,li->soname,h);
      }
      if ( h ) {
        get_table = dlsym( h, "_libfoo_get_impl_table" );
        fprintf(stderr,"%s: dlsym( %p, \"_libfoo_get_impl_table\") = %p\n",__FUNCTION__,h,get_table);

        if ( get_table == &_libfoo_get_impl_table ) {
          li->table = &default_table;
          li->opt_table = &default_opt_table;
          break;
        }
        else {
          get_opt_table = dlsym( h, "_libfoo_get_opt_impl_table" );
          fprintf(stderr,"%s: dlsym( %p, \"_libfoo_get_opt_impl_table\") = %p\n",__FUNCTION__,h,get_opt_table);
        
          if ( get_table && get_opt_table ) {
            li->table = get_table();
            li->opt_table = get_opt_table();
            break;
          }
        }
      }
    }
    li = li->next;
  }

  fprintf(stderr,"%s: Selected { soname=\"%s\", origin=\"%s\", table=%p, opt_table=%p, next=%p }\n",__FUNCTION__,li->soname,li->origin,li->table,li->opt_table,li->next);

  table = li->table;
  opt_table = li->opt_table;

  if ( !table || !opt_table ) {
    fprintf(stderr,"ERROR: Could not find any valid implementation (%p, %p)!\n",table,opt_table);
    abort();
  }
}

__attribute__((visibility("default")))
__libfoo_impl_table *_libfoo_get_impl_table( void )
{
  if ( !table ) get_tables();
  return table;
}

__attribute__((visibility("default")))
__libfoo_opt_impl_table *_libfoo_get_opt_impl_table( void )
{
  static int called = 0;
  if ( !opt_table ) get_tables();
  return opt_table;
}

__attribute__((constructor))
static void detect_only_vars( void )
{
  if ( &foo_version_string == &__foo_version_string ) return;
  __libfoo_impl_table *t = _libfoo_get_impl_table();
  t->init("Direct");
}

