CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS ?=

LIBRARY = libcpu.a
CORE_OBJECT = build/cpu.o
MONITOR = build/cpu-monitor

.PHONY: all core monitor test clean

all: core monitor

core: $(LIBRARY)

$(LIBRARY): $(CORE_OBJECT)
	ar rcs $@ $^

monitor: $(MONITOR)

$(MONITOR): examples/monitor.c $(LIBRARY)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -L. -lcpu -lncurses $(LDFLAGS)

test: build/test_cpu
	./build/test_cpu

build/test_cpu: tests/test_cpu.c $(LIBRARY)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -L. -lcpu $(LDFLAGS)

build/cpu.o: src/cpu.c include/cpu.h
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf build $(LIBRARY)
