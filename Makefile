impls:=lib/libfoo-1.so lib/libfoo-2.so lib/tracefoo.so
bins:=bin/test bin/test-shared bin/test_foo bin/test_foo-1 bin/test_foo-2 bin/test_optional bin/test_optional-1 bin/test_optional-2 bin/test_version bin/test_version-1 bin/test_version-2
all: lib/libfoo.so $(impls) $(bins)
.PHONY: all

CFLAGS = -g -Iinclude/

trace_sources:=$(wildcard src/trace/*.c)
trace_objects:=$(patsubst %.c,%.o,$(trace_sources))
nonshared_sources:=$(wildcard src/nonshared/*.c)
nonshared_objects:=$(patsubst %.c,%.o,$(nonshared_sources))
shared_sources:=$(wildcard src/shared/*.c)
shared_objects:=$(patsubst %.c,%.o,$(shared_sources))
impl1_sources:=$(wildcard src/impl/1/*.c)
impl1_objects:=$(patsubst %.c,%.o,$(impl1_sources))
impl2_sources:=$(wildcard src/impl/2/*.c)
impl2_objects:=$(patsubst %.c,%.o,$(impl2_sources))
impl_objects:=$(impl1_objects) $(impl2_objects)
lib_objects:=$(trace_objects) $(nonshared_objects) $(shared_objects) $(impl_objects)

$(nonshared_objects): include/libfoo_nonshared.h

$(lib_objects): CFLAGS+=-fPIC
$(lib_objects): include/libfoo_shared.h

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

bin/test: src/test.c include/libfoo.h lib/libfoo.so
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $< -Llib -lfoo -Wl,-rpath=\$$ORIGIN/../lib

bin/test-shared: src/test.c include/libfoo.h lib/libfoo.so
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $< -Llib -lfoo_shared -Wl,-rpath=\$$ORIGIN/../lib

bin/test_foo: src/test_foo.c include/libfoo.h lib/libfoo.so
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $< -Llib -lfoo -Wl,-rpath=\$$ORIGIN/../lib

bin/test_foo-1: src/test_foo.c include/libfoo.h lib/libfoo-1.so
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $< -Llib -lfoo -lfoo-1 -Wl,-rpath=\$$ORIGIN/../lib

bin/test_foo-2: src/test_foo.c include/libfoo.h lib/libfoo-2.so
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $< -Llib -lfoo -lfoo-2 -Wl,-rpath=\$$ORIGIN/../lib

bin/test_optional: src/test_optional.c include/libfoo.h lib/libfoo.so
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $< -Llib -lfoo -Wl,-rpath=\$$ORIGIN/../lib

bin/test_optional-1: src/test_optional.c include/libfoo.h lib/libfoo.so lib/libfoo-1.so
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $< -Llib -lfoo -lfoo-1 -Wl,-rpath=\$$ORIGIN/../lib

bin/test_optional-2: src/test_optional.c include/libfoo.h lib/libfoo.so lib/libfoo-2.so
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $< -Llib -lfoo -lfoo-2 -Wl,-rpath=\$$ORIGIN/../lib

bin/test_version: src/test_version.c include/libfoo.h lib/libfoo.so
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $< -Llib -lfoo -Wl,-rpath=\$$ORIGIN/../lib

bin/test_version-1: src/test_version.c include/libfoo.h lib/libfoo.so lib/libfoo-1.so
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $< -Llib -lfoo -lfoo-1 -Wl,-rpath=\$$ORIGIN/../lib

bin/test_version-2: src/test_version.c include/libfoo.h lib/libfoo.so lib/libfoo-2.so
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $< -Llib -lfoo -lfoo-2 -Wl,-rpath=\$$ORIGIN/../lib

lib/libfoo_nonshared.a: $(nonshared_objects)
	$(AR) cru $@ $^

lib/libfoo_shared.so: $(shared_objects)
	$(CC) -shared -o $@ $^ -ldl -Wl,-rpath=\$$ORIGIN,-soname=$(notdir $@)

lib/foo_impl1.so: $(impl1_objects) | lib/libfoo_shared.so
	$(CC) -shared -o $@ $^ -Llib -lfoo_shared -ldl -Wl,-rpath=\$$ORIGIN,-soname=$(notdir $@)

lib/foo_impl2.so: $(impl2_objects) | lib/libfoo_shared.so
	$(CC) -shared -o $@ $^ -Llib -lfoo_shared -ldl -Wl,-rpath=\$$ORIGIN,-soname=$(notdir $@)

lib/libfoo.so: lib/libfoo_nonshared.a lib/libfoo_shared.so
	touch $@

lib/libfoo-1.so: CFLAGS+=-fPIC
lib/libfoo-1.so: src/impl/foo_impl.c include/libfoo_shared.h lib/libfoo.so lib/foo_impl1.so
	$(CC) $(CFLAGS) -DFOO_IMPL_SO=foo_impl1.so -DFOO_NAME=$(notdir $@) -shared -o $@ $< -ldl -Wl,-rpath=\$$ORIGIN,-soname=$(notdir $@)

lib/libfoo-2.so: CFLAGS+=-fPIC
lib/libfoo-2.so: src/impl/foo_impl.c include/libfoo_shared.h lib/libfoo.so lib/foo_impl2.so
	$(CC) $(CFLAGS) -DFOO_IMPL_SO=foo_impl2.so -DFOO_NAME=$(notdir $@) -shared -o $@ $< -ldl -Wl,-rpath=\$$ORIGIN,-soname=$(notdir $@)

lib/tracefoo.so: $(trace_objects) | lib/libfoo_shared.so
	$(CC) -shared -o $@ $^ -Llib -lfoo_shared -ldl -Wl,-rpath=\$$ORIGIN,-soname=$(notdir $@)

clean:
	$(RM) bin/test* lib/libfoo_nonshared.a lib/libfoo_shared.so lib/foo_impl*.so lib/libfoo-*.so lib/tracefoo.so $(lib_objects)

.PHONY: clean
