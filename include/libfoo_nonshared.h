#ifndef _FOO_NONSHARED_H_
#define _FOO_NONSHARED_H_

#include "libfoo.h"
#include "libfoo_shared.h"

extern __libfoo_impl_table *__libfoo_impl_table_ptr __attribute__((visibility("hidden")));
extern __libfoo_opt_impl_table *__libfoo_opt_impl_table_ptr __attribute__((visibility("hidden")));

#endif
