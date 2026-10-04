CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS ?=

LIBRARY = libcpu.a
CORE_OBJECT = build/cpu.o
CORE_OBJECTS = build/cpu.o build/cpu_info.o build/cpu_usage.o
MONITOR = build/cpu-monitor

.PHONY: all core monitor test test-portable clean

all: core monitor

core: $(LIBRARY)

$(LIBRARY): $(CORE_OBJECTS)
	ar rcs $@ $^

monitor: $(MONITOR)

$(MONITOR): examples/monitor.c $(LIBRARY)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -L. -lcpu -lncurses $(LDFLAGS)

test: build/test_cpu
	./build/test_cpu

test-portable:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) -DCPU_READER_DISABLE_ASM" test

build/test_cpu: tests/test_cpu.c $(LIBRARY)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -L. -lcpu $(LDFLAGS)

build/cpu.o: src/cpu.c include/cpu.h src/cpu_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/cpu_info.o: src/cpu_info.c include/cpu.h src/cpu_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/cpu_usage.o: src/cpu_usage.c include/cpu.h src/cpu_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf build $(LIBRARY)
