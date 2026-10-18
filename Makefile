CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS ?=
SANITIZER_FLAGS ?= -fsanitize=address,undefined -fno-omit-frame-pointer
SECURITY_CFLAGS ?= -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE \
	-Wformat=2 -Wformat-security
SECURITY_LDFLAGS ?= -Wl,-z,relro,-z,now -pie

LIBRARY = libcpu.a
CORE_OBJECT = build/cpu.o
CORE_OBJECTS = build/cpu.o build/cpu_info.o build/cpu_telemetry.o build/cpu_usage.o build/cpu_diagnostics.o
MONITOR = build/cpu-monitor
GTK_APP = build/cpu-reader-gtk
GTK_CFLAGS = $(shell pkg-config --cflags gtk4)
GTK_LIBS = $(shell pkg-config --libs gtk4)
DEB_ARCH ?= $(shell dpkg --print-architecture)
PACKAGE_VERSION ?= 0.1.0+git$(shell git rev-parse --short HEAD 2>/dev/null || echo local)
DEB_PACKAGE = build/cpu-reader_$(PACKAGE_VERSION)_$(DEB_ARCH).deb
DEB_ROOT = build/deb-root

.PHONY: all core monitor gtk-monitor test test-diagnostics test-gtk test-gtk-ui package-deb test-portable test-sanitize test-security \
	telemetry benchmark clean

all: core monitor

core: $(LIBRARY)

$(LIBRARY): $(CORE_OBJECTS)
	ar rcs $@ $^

monitor: $(MONITOR)

gtk-monitor: $(GTK_APP)

test-gtk: $(GTK_APP)
	sh tests/test_gtk_app.sh $(GTK_APP)

test-gtk-ui: $(GTK_APP)
	sh tests/test_gtk_ui.sh $(GTK_APP)

package-deb: $(DEB_PACKAGE)

$(GTK_APP): examples/gtk_monitor.c $(LIBRARY)
	@pkg-config --exists gtk4 || { echo "GTK4 development files missing; install libgtk-4-dev and pkg-config." >&2; exit 1; }
	@mkdir -p build
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -DAPP_VERSION=\"$(PACKAGE_VERSION)\" -o $@ $< -L. -lcpu $(GTK_LIBS) $(LDFLAGS)

$(DEB_PACKAGE): $(GTK_APP) packaging/debian/cpu-reader.desktop packaging/icons/cpu-reader.svg packaging/metainfo/io.github.mnl_nox.CPUReader.metainfo.xml README.md LICENSE
	@command -v dpkg-deb >/dev/null || { echo "dpkg-deb is required to build Debian packages." >&2; exit 1; }
	rm -rf $(DEB_ROOT)
	install -Dm755 $(GTK_APP) $(DEB_ROOT)/usr/bin/cpu-reader
	install -Dm644 packaging/debian/cpu-reader.desktop $(DEB_ROOT)/usr/share/applications/cpu-reader.desktop
	install -Dm644 packaging/icons/cpu-reader.svg $(DEB_ROOT)/usr/share/icons/hicolor/scalable/apps/cpu-reader.svg
	install -Dm644 packaging/metainfo/io.github.mnl_nox.CPUReader.metainfo.xml $(DEB_ROOT)/usr/share/metainfo/io.github.mnl_nox.CPUReader.metainfo.xml
	install -Dm644 README.md $(DEB_ROOT)/usr/share/doc/cpu-reader/README.md
	install -Dm644 LICENSE $(DEB_ROOT)/usr/share/doc/cpu-reader/copyright
	printf '%s\n' \
		'Package: cpu-reader' \
		'Version: $(PACKAGE_VERSION)' \
		'Section: utils' \
		'Priority: optional' \
		'Architecture: $(DEB_ARCH)' \
		'Maintainer: CPU Reader contributors <maintainers@users.noreply.github.com>' \
		'Depends: libgtk-4-1, libglib2.0-0 | libglib2.0-0t64, libc6' \
		'Description: GTK4 desktop CPU monitor for Linux' \
		' CPU usage, CPU topology, temperature and current frequency dashboard.' \
		> $(DEB_ROOT)/DEBIAN-control
	mkdir -p $(DEB_ROOT)/DEBIAN
	mv $(DEB_ROOT)/DEBIAN-control $(DEB_ROOT)/DEBIAN/control
	dpkg-deb --root-owner-group --build $(DEB_ROOT) $(DEB_PACKAGE)

$(MONITOR): examples/monitor.c $(LIBRARY)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -L. -lcpu -lncurses $(LDFLAGS)

test: build/test_cpu build/test_platform build/test_diagnostics build/test_concurrency
	./build/test_cpu
	./build/test_platform
	./build/test_diagnostics
	./build/test_concurrency

test-diagnostics: build/test_diagnostics
	./build/test_diagnostics

test-portable:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS)" test

test-sanitize:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) $(SANITIZER_FLAGS)" \
		LDFLAGS="$(LDFLAGS) $(SANITIZER_FLAGS)" test

test-security:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) $(SECURITY_CFLAGS)" \
		LDFLAGS="$(LDFLAGS) $(SECURITY_LDFLAGS)" test

benchmark: build/bench_cpu
	./build/bench_cpu

build/bench_cpu: bench/bench_cpu.c $(LIBRARY)
	@mkdir -p build
	$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=200809L -o $@ $< -L. -lcpu $(LDFLAGS)

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

build/cpu_telemetry.o: src/cpu_telemetry.c include/cpu.h src/cpu_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/cpu_usage.o: src/cpu_usage.c include/cpu.h src/cpu_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/cpu_diagnostics.o: src/cpu_diagnostics.c include/cpu_diagnostics.h include/cpu.h
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf build $(LIBRARY)

build/test_diagnostics: tests/test_diagnostics.c $(LIBRARY)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -L. -lcpu $(LDFLAGS)

build/test_concurrency: tests/test_concurrency.c $(LIBRARY)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -L. -lcpu $(LDFLAGS) -pthread
