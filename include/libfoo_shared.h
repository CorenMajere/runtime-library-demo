#ifndef _FOO_SHARED_H_
#define _FOO_SHARED_H_

#define LIBFOO_SHARED_MAJOR 1
#define LIBFOO_SHARED_MINOR 2

#define LIBFOO_SHARED_OPT_MAJOR 1
#define LIBFOO_SHARED_OPT_MINOR 2

typedef const char *(*__libfoo_init_t)( const char * );
typedef void(*__libfoo_opt_init_t)();

typedef struct {
  struct {
    int major;
    int minor;
  } version;
  __libfoo_init_t init;
  const char *(*foo)( void );
} __libfoo_impl_table;

typedef struct {
  struct {
    int major;
    int minor;
  } version;
  __libfoo_opt_init_t init;
  const char *(*foo_optional)( void );
} __libfoo_opt_impl_table;

__libfoo_impl_table *_libfoo_get_impl_table( void ) __attribute__((visibility("default")));
__libfoo_opt_impl_table *_libfoo_get_opt_impl_table( void ) __attribute__((visibility("default")));

typedef struct __libfoo_impl_list {
  const char *soname;
  const char *origin;
  __libfoo_impl_table *table;
  __libfoo_opt_impl_table *opt_table;
  struct __libfoo_impl_list *next;
} __libfoo_impl_list;

void _libfoo_register_impl( __libfoo_impl_list ** ) __attribute__((visibility("default")));

#endif
