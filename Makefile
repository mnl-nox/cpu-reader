CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS ?=
SANITIZER_FLAGS ?= -fsanitize=address,undefined -fno-omit-frame-pointer
SECURITY_CFLAGS ?= -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE \
	-Wformat=2 -Wformat-security
SECURITY_LDFLAGS ?= -Wl,-z,relro,-z,now -pie

LIBRARY = libcpu.a
CORE_OBJECT = build/cpu.o
CORE_OBJECTS = build/cpu.o build/cpu_info.o build/cpu_usage.o
MONITOR = build/cpu-monitor

.PHONY: all core monitor test test-portable test-sanitize test-security \
	telemetry clean

all: core monitor

core: $(LIBRARY)

$(LIBRARY): $(CORE_OBJECTS)
	ar rcs $@ $^

monitor: $(MONITOR)

$(MONITOR): examples/monitor.c $(LIBRARY)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -L. -lcpu -lncurses $(LDFLAGS)

test: build/test_cpu build/test_platform
	./build/test_cpu
	./build/test_platform

test-portable:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) -DCPU_READER_DISABLE_ASM" test

test-sanitize:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) $(SANITIZER_FLAGS)" \
		LDFLAGS="$(LDFLAGS) $(SANITIZER_FLAGS)" test

test-security:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) $(SECURITY_CFLAGS)" \
		LDFLAGS="$(LDFLAGS) $(SECURITY_LDFLAGS)" test

telemetry: test
	@mkdir -p build
	@printf '{\n  "project": "cpu-reader",\n  "compiler": "%s",\n  "platform": "%s",\n  "test": "make test",\n  "status": "passed"\n}\n' \
		"$(CC)" "$$(uname -s)-$$(uname -m)" > build/telemetry.json
	@printf 'Telemetry written to build/telemetry.json\n'

build/test_cpu: tests/test_cpu.c $(LIBRARY)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -L. -lcpu $(LDFLAGS)

build/test_platform: tests/test_platform.c $(LIBRARY)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -L. -lcpu $(LDFLAGS) -pthread

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
