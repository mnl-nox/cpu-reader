# CPU Reader - Deployment & Packaging Guide

**Version:** 1.0  
**Date:** 2026-09-04  
**Target:** Debian (.deb), RPM, Snap, AppImage

---

## 1. Overview

CPU Reader will be distributed through multiple channels:

- **Debian/Ubuntu**: Native .deb package
- **Fedora/RHEL**: Native .rpm package
- **Universal**: Snap, AppImage
- **Arch Linux**: PKGBUILD
- **Installation**: Terminal commands or GUI package manager

---

## 2. Debian Package Structure

### 2.1 debian/control

```
Source: cpu-reader
Section: utils
Priority: optional
Maintainer: CPU Reader Team <team@example.com>
Build-Depends:
 debhelper-compat (= 13),
 cmake,
 qt6-base-dev,
 qt6-charts-dev,
 qt6-tools-dev,
 qt6-tools-dev-tools
Standards-Version: 4.6.2
Homepage: https://github.com/mnl-nox/cpu-reader
Vcs-Git: https://github.com/mnl-nox/cpu-reader.git
Vcs-Browser: https://github.com/mnl-nox/cpu-reader
Rules-Requires-Root: no

Package: cpu-reader
Architecture: any
Depends:
 ${shlibs:Depends},
 ${misc:Depends}
Recommends:
 fonts-dejavu
Description: Real-time CPU monitoring dashboard
 CPU Reader is a modern system monitoring application that provides
 real-time CPU usage statistics, temperature readings, and performance
 metrics through an intuitive Qt6-based graphical interface.
 .
 Features:
  - Real-time CPU usage monitoring
  - Interactive performance charts
  - System tray integration
  - Metrics export (CSV, JSON, PDF)
  - Wayland support
  - Multi-threaded architecture
```

### 2.2 debian/rules

```makefile
#!/usr/bin/make -f

export DH_VERBOSE = 1

%:
	dh $@ -Scmake

override_dh_auto_configure:
	dh_auto_configure -- -DCMAKE_BUILD_TYPE=Release

override_dh_auto_build:
	dh_auto_build

override_dh_auto_install:
	dh_auto_install
	# Install additional files
	dh_install

override_dh_missing:
	dh_missing --fail-missing
```

### 2.3 debian/changelog

```
cpu-reader (0.2.0-1) unstable; urgency=medium

  * Add Qt6 GUI with dashboard
  * Real-time CPU monitoring with charts
  * System tray integration
  * Metrics export functionality
  * Wayland support
  * Multi-platform installer

 -- CPU Reader Team <team@example.com>  Thu, 04 Sep 2026 12:00:00 +0000

cpu-reader (0.1.1-1) unstable; urgency=low

  * Remove emoji characters from documentation
  * Add CHANGELOG.md
  * Improve accessibility

 -- CPU Reader Team <team@example.com>  Thu, 04 Sep 2026 10:00:00 +0000

cpu-reader (0.1.0-1) unstable; urgency=low

  * Initial release
  * Core C99 library
  * Unit tests
  * ncurses monitor
  * Comprehensive documentation

 -- CPU Reader Team <team@example.com>  Thu, 04 Sep 2026 08:00:00 +0000
```

### 2.4 debian/copyright

```
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: cpu-reader
Upstream-Contact: CPU Reader Team <team@example.com>
Source: https://github.com/mnl-nox/cpu-reader

Files: *
Copyright: 2026 CPU Reader Team
License: MIT

License: MIT
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 .
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 .
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

### 2.5 debian/postinst (Post-install script)

```bash
#!/bin/sh
set -e

# Post-installation script for cpu-reader

# Update desktop database
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database /usr/share/applications
fi

# Update icon cache
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -f -t /usr/share/icons/hicolor || true
fi

# Update mime database
if command -v update-mime-database >/dev/null 2>&1; then
    update-mime-database /usr/share/mime || true
fi

exit 0
```

### 2.6 debian/postrm (Post-removal script)

```bash
#!/bin/sh
set -e

# Post-removal script for cpu-reader

if [ "$1" = "purge" ]; then
    # Clean up config files
    rm -rf /etc/cpu-reader || true
fi

# Update desktop database
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database /usr/share/applications || true
fi

# Update icon cache
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -f -t /usr/share/icons/hicolor || true
fi

exit 0
```

### 2.7 debian/preinst (Pre-install script)

```bash
#!/bin/sh
set -e

# Pre-installation script for cpu-reader

# Check if running as root or via sudo (dpkg checks this automatically)
# This is optional for user-level installations

exit 0
```

---

## 3. Building the Debian Package

### 3.1 Build Script (scripts/package-deb.sh)

```bash
#!/bin/bash
set -e

echo "Building CPU Reader Debian package..."

# Variables
VERSION="0.2.0"
DISTRO="$(lsb_release -cs)"
BUILD_DIR="build"
PACKAGE_DIR="${BUILD_DIR}/cpu-reader-${VERSION}"

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Clean previous build
if [ -d "${PACKAGE_DIR}" ]; then
    rm -rf "${PACKAGE_DIR}"
fi

# Copy source code
cp -r .. "${PACKAGE_DIR}"
cd "${PACKAGE_DIR}"

# Build the package
debuild -us -uc

echo "Package built successfully!"
echo "Output: ${BUILD_DIR}/cpu-reader_${VERSION}_amd64.deb"
```

### 3.2 Prerequisites for Building

```bash
# Install build dependencies
sudo apt-get install -y \
    build-essential \
    cmake \
    debhelper \
    devscripts \
    qt6-base-dev \
    qt6-charts-dev \
    qt6-tools-dev \
    git

# Install signing tools (optional, for signing packages)
sudo apt-get install -y gnupg ubuntu-dev-tools
```

### 3.3 Build Commands

```bash
# Method 1: Using debuild
cd /path/to/cpu-reader
debuild -us -uc

# Method 2: Using dpkg-buildpackage
dpkg-buildpackage -us -uc

# Method 3: Using cmake + cpack
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cpack -G DEB
```

---

## 4. Alternative: cpack Integration

### 4.1 CMakeLists.txt (Packaging section)

```cmake
# Packaging
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "cpu-reader")
set(CPACK_PACKAGE_VERSION "0.2.0")
set(CPACK_PACKAGE_VENDOR "CPU Reader Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Real-time CPU monitoring dashboard")
set(CPACK_PACKAGE_CONTACT "team@example.com")

# DEB specific
set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libc6, libqt6core6, libqt6gui6, libqt6widgets6, libqt6charts6")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER
    "CPU Reader Team <team@example.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/mnl-nox/cpu-reader")

# Desktop file
install(FILES gui/cpu-reader.desktop
        DESTINATION share/applications)

# Icons
install(DIRECTORY gui/resources/icons/
        DESTINATION share/icons/hicolor)

include(CPack)
```

---

## 5. AppImage Packaging

### 5.1 linuxdeploy Configuration

```bash
#!/bin/bash

# Build application
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Create AppImage
linuxdeploy-x86_64.AppImage \
    --appimage-extract-and-run \
    --appdir=AppDir \
    --executable=bin/cpu-reader \
    --desktop-file=../gui/cpu-reader.desktop \
    --icon-file=../gui/resources/icons/cpu-reader-256x256.png \
    --output appimage

# Result: cpu-reader-0.2.0-x86_64.AppImage
```

---

## 6. Installation Methods

### 6.1 Terminal Installation

```bash
# From .deb file
sudo apt install ./cpu-reader_0.2.0_amd64.deb

# From PPA (when available)
sudo apt-add-repository ppa:mnl-nox/cpu-reader
sudo apt update
sudo apt install cpu-reader

# From Snap
sudo snap install cpu-reader

# From AppImage
chmod +x cpu-reader-0.2.0-x86_64.AppImage
./cpu-reader-0.2.0-x86_64.AppImage
```

### 6.2 GUI Installation (Desktop)

1. Download .deb file
2. Double-click in file manager
3. Click "Install" button
4. Enter password if prompted
5. Application appears in launcher

### 6.3 Uninstallation

```bash
# Remove from terminal
sudo apt remove cpu-reader

# Remove with configuration
sudo apt purge cpu-reader

# From Snap
snap remove cpu-reader

# AppImage (just delete the file)
rm cpu-reader-0.2.0-x86_64.AppImage
```

---

## 7. Distribution Channels

### 7.1 GitHub Releases

```bash
# Create release with assets
gh release create v0.2.0 \
    --title "CPU Reader v0.2.0" \
    --notes "Release notes..." \
    build/cpu-reader_0.2.0_amd64.deb \
    build/cpu-reader-0.2.0.tar.gz \
    build/cpu-reader-0.2.0-x86_64.AppImage
```

### 7.2 Debian Repository

```bash
# Host on Launchpad PPA or similar
# Instructions for PPA setup:

# 1. Create account on launchpad.net
# 2. Import GPG key
# 3. Create PPA
# 4. Upload via dput

dput ppa:mnl-nox/cpu-reader \
    ../cpu-reader_0.2.0_source.changes
```

### 7.3 Snap Store

```bash
# Build Snap
snapcraft

# Push to store
snapcraft push cpu-reader_0.2.0_amd64.snap --release=stable
```

---

## 8. System Integration

### 8.1 Systemd Service (Optional)

File: `packaging/debian/cpu-reader.service`

```ini
[Unit]
Description=CPU Reader Monitoring Service
After=network.target
Wants=cpu-reader.timer

[Service]
Type=simple
ExecStart=/usr/bin/cpu-reader --daemon
Restart=always
RestartSec=10
User=_cpu-reader
PrivateTmp=yes
NoNewPrivileges=true

[Install]
WantedBy=multi-user.target
```

Installation:

```bash
sudo systemctl enable cpu-reader.service
sudo systemctl start cpu-reader.service
```

---

## 9. Dependency Management

### 9.1 Build Dependencies

- cmake (>= 3.24)
- gcc/clang with C99 support
- qt6-base-dev
- qt6-charts-dev
- qt6-tools-dev

### 9.2 Runtime Dependencies

- libc6
- libqt6core6
- libqt6gui6
- libqt6widgets6
- libqt6charts6

### 9.3 Optional Runtime Dependencies

- libqt6dbus6 (for D-Bus notifications)
- libqt6network6 (for future network features)

---

## 10. Testing the Package

### 10.1 Installation Testing

```bash
# Create test environment
mkdir test-install
cd test-install

# Extract .deb
ar x ../cpu-reader_0.2.0_amd64.deb
tar -xf data.tar.xz

# Verify structure
find . -type f

# Test in VM or container
docker run -it ubuntu:22.04 bash
# Copy .deb and install
```

### 10.2 Functionality Testing

```bash
# After installation
cpu-reader --version
cpu-reader --help

# Run GUI
cpu-reader &

# Check in system menu
# Verify tray icon appears
# Test metrics collection
# Test export functionality
```

---

## 11. CI/CD Integration

### 11.1 GitHub Actions Workflow (.github/workflows/package.yml)

```yaml
name: Package

on:
  push:
    tags:
      - "v*"

jobs:
  build-deb:
    runs-on: ubuntu-22.04
    steps:
      - uses: actions/checkout@v3

      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y \
            cmake qt6-base-dev qt6-charts-dev qt6-tools-dev

      - name: Build package
        run: |
          mkdir build && cd build
          cmake -DCMAKE_BUILD_TYPE=Release ..
          cpack -G DEB

      - name: Upload artifacts
        uses: actions/upload-artifact@v3
        with:
          name: packages
          path: build/*.deb

      - name: Create Release
        uses: actions/create-release@v1
        with:
          tag_name: ${{ github.ref }}
          release_name: CPU Reader ${{ github.ref }}
          files: build/*.deb
```

---

## 12. Troubleshooting

### Common Issues

**Issue**: "Qt6 not found" during build

```bash
# Solution: Install Qt6
sudo apt-get install qt6-base-dev qt6-charts-dev

# Or set CMAKE path
cmake -DQt6_DIR=/usr/lib/cmake/Qt6 ..
```

**Issue**: ".deb file not installable"

```bash
# Check dependencies
dpkg -I cpu-reader_0.2.0_amd64.deb

# Install missing dependencies
sudo apt-get install -f

# Install the package
sudo dpkg -i cpu-reader_0.2.0_amd64.deb
```

**Issue**: "Permission denied" when running

```bash
# Make executable
chmod +x /usr/bin/cpu-reader

# Or reinstall
sudo apt-get install --reinstall cpu-reader
```

---

## 13. Security Checklist

- [ ] Package built from verified source
- [ ] GPG signature on package
- [ ] Dependencies verified
- [ ] No world-writable files
- [ ] No hardcoded credentials
- [ ] Security scanning (optional)

---

## 14. Release Checklist

- [ ] Version bumped in CMakeLists.txt
- [ ] CHANGELOG.md updated
- [ ] Package built successfully
- [ ] Installation testing passed
- [ ] Functionality testing passed
- [ ] Documentation updated
- [ ] Git tag created
- [ ] GitHub release created
- [ ] .deb package uploaded
- [ ] AppImage generated
- [ ] Snap package built
- [ ] Announcement posted

---

**Next Steps:**

1. Set up CMake build system
2. Build and test .deb package
3. Set up CI/CD for automated packaging
4. Deploy to GitHub Releases
5. Publish to distribution channels
