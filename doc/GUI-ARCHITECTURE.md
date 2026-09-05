# CPU Reader - GUI Architecture Document

**Version:** 2.0  
**Date:** 2026-09-04  
**Status:** Design Phase  
**Target:** v0.2.0 (GUI with Dashboard)

---

## 1. Overview

Extend CPU Reader with a modern **Qt6-based GUI application** featuring:
- Real-time CPU monitoring dashboard with interactive graphs
- Multi-threaded performance metrics collection
- Wayland-compatible interface (with X11 fallback)
- System tray integration for background monitoring
- Installable via terminal and desktop package (.deb)

### Architecture Layers

```
┌─────────────────────────────────────────┐
│  GUI Layer (Qt6)                        │
│  - Main Window                          │
│  - Dashboard with Charts                │
│  - Settings Dialog                      │
│  - System Tray                          │
├─────────────────────────────────────────┤
│  Business Logic Layer                   │
│  - Data Collection Service              │
│  - Metrics Processor                    │
│  - Export/Report Generator              │
├─────────────────────────────────────────┤
│  Hardware Abstraction (libcpu)          │
│  - Existing C99 Library                 │
│  - cpu_get_info()                       │
│  - cpu_get_usage()                      │
│  - cpu_get_temperature()                │
│  - cpu_get_clock_speed()                │
└─────────────────────────────────────────┘
```

---

## 2. Project Structure (Post v0.1.1)

```
cpu-reader/
├── CMakeLists.txt                    [NEW - Replaces/supplements Makefile]
├── Makefile                          [KEPT - C library build]
├── src/                              [EXISTING - C99 Library]
│   ├── cpu.c
│   ├── cpu_info.c
│   ├── cpu_usage.c
│   ├── cpu_internal.h
│
├── include/
│   └── cpu.h                         [EXISTING - Public API]
│
├── gui/                              [NEW - Qt6 Application]
│   ├── CMakeLists.txt                [Qt6 build config]
│   ├── src/
│   │   ├── main.cpp                  [Entry point]
│   │   ├── mainwindow.h/cpp          [Main window with menu]
│   │   ├── dashboard.h/cpp           [Dashboard widget]
│   │   ├── cpuchart.h/cpp            [CPU usage chart (QChart)]
│   │   ├── systemtray.h/cpp          [System tray integration]
│   │   ├── metrics_collector.h/cpp   [Data collection thread]
│   │   ├── report_generator.h/cpp    [Export/report generation]
│   │   └── preferences.h/cpp         [Settings dialog]
│   │
│   └── resources/
│       ├── icons/                    [Application icons]
│       │   ├── cpu-reader.svg
│       │   ├── cpu-reader-16x16.png
│       │   ├── cpu-reader-64x64.png
│       │   └── cpu-reader-256x256.png
│       │
│       └── ui/
│           ├── mainwindow.ui         [Qt Designer UI files]
│           ├── dashboard.ui
│           └── preferences.ui
│
├── packaging/                        [NEW - Distribution packages]
│   ├── cpu-reader.spec              [RPM spec file]
│   ├── debian/                      [DEB package metadata]
│   │   ├── control                  [Package info]
│   │   ├── rules                    [Build rules]
│   │   ├── changelog                [Version history]
│   │   ├── copyright                [License]
│   │   ├── cpu-reader.desktop       [Desktop entry]
│   │   ├── cpu-reader.service       [Systemd service]
│   │   └── postinst                 [Post-install script]
│   │
│   └── docker/
│       ├── Dockerfile               [Container build]
│       └── Dockerfile.build          [CI build container]
│
├── doc/
│   ├── GUI-ARCHITECTURE.md          [This file]
│   ├── GUI-IMPLEMENTATION.md        [Implementation guide]
│   ├── GUI-UX-DESIGN.md            [UX specifications]
│   ├── DEPLOYMENT.md                [Installation & deployment]
│   └── [...existing docs...]
│
├── examples/
│   ├── monitor.c                    [EXISTING - ncurses TUI]
│   ├── [...future examples...]
│
├── tests/
│   ├── test_cpu.c                   [EXISTING - C library tests]
│   ├── test_metrics_collector.cpp   [NEW - GUI component tests]
│   └── [...]
│
├── scripts/
│   ├── build.sh                     [Build all targets]
│   ├── build-gui.sh                 [Build GUI only]
│   ├── package-deb.sh               [Create .deb package]
│   ├── package-rpm.sh               [Create .rpm package]
│   └── install.sh                   [Installation script]
│
├── .github/workflows/               [CI/CD]
│   ├── build.yml
│   ├── package.yml
│   └── release.yml
│
└── [...existing files...]
```

---

## 3. Technology Stack

### Core
- **Language:** C99 (library), C++17 (GUI)
- **Library:** Qt 6.4+ (LTS)
- **Build:** CMake 3.24+
- **Package:** Debian (.deb), RPM (.rpm)

### GUI Components
- **Qt6Core** - Core framework
- **Qt6Gui** - Graphics
- **Qt6Widgets** - UI widgets
- **Qt6Charts** - Chart/graph support
- **Qt6DBus** - D-Bus integration (system notifications)
- **Qt6Svg** - SVG rendering for icons

### Build & Packaging
- **CMake** - Cross-platform build
- **dpkg-dev** - Debian packaging
- **rpmbuild** - RPM packaging
- **debhelper** - Debian helper utilities

---

## 4. Key Features

### 4.1 Dashboard

**Real-time Metrics Display:**
- CPU usage percentage (aggregated)
- Per-core CPU usage (if RF-013 implemented)
- System temperature (when available)
- Clock speed (current and max)
- Active processes
- Memory usage (future extension)

**Interactive Charts:**
- Time-series CPU usage graph (last 1h, 24h, 7d)
- Per-core usage breakdown (stacked area chart)
- Temperature trend line
- Clock speed history

### 4.2 System Tray Integration
- Minimize to system tray
- Quick-access stats on hover
- Right-click menu:
  - Show/Hide main window
  - Start/Stop monitoring
  - Export current metrics
  - Preferences
  - Exit

### 4.3 Reporting & Export
- Export metrics to CSV
- Generate PDF reports (charts + stats)
- Historical data retention (configurable)
- Daily/weekly/monthly summaries

### 4.4 Settings
- Update interval (1s, 5s, 10s, 30s, 60s)
- Data retention period (1h, 24h, 7d, 30d)
- Chart colors and styles
- Notifications (thresholds for CPU/temp)
- Start on boot (systemd integration)
- Dark/Light theme

---

## 5. Data Flow

### Collection Pipeline

```
┌─────────────────────────────────────┐
│ MetricsCollector (QThread)          │
│ - Reads from libcpu                 │
│ - 1s interval (configurable)        │
└──────────────┬──────────────────────┘
               │ emit metricsUpdated()
               ▼
┌─────────────────────────────────────┐
│ Dashboard Widget                    │
│ - Updates charts                    │
│ - Stores data points                │
│ - Refreshes UI                      │
└──────────────┬──────────────────────┘
               │
               ▼
         ┌──────────────┐
         │  Data Store  │
         │ (In-memory   │
         │  + SQLite)   │
         └──────────────┘
```

### Interaction Flow

```
User Action
    │
    ▼
GUI Event Handler (Qt Slot)
    │
    ▼
Business Logic (Service Layer)
    │
    ▼
libcpu C Library Call
    │
    ▼
/proc or /sys Read
    │
    ▼
Return Data
    │
    ▼
Cache & Signal Update
    │
    ▼
Update UI (Qt Signal)
```

---

## 6. Threading Model

### Main Thread
- Qt event loop
- UI rendering
- User interactions

### Worker Thread (MetricsCollector)
- Reads from libcpu on 1s interval
- Emits signals (thread-safe via Qt)
- No blocking calls
- CPU: <1% when idle

### Database Thread (Optional)
- Writes metrics to SQLite
- Asynchronous, non-blocking
- Historical data management

---

## 7. Wayland Compatibility

### Requirements
- Qt 6.4+ with Wayland plugin
- On Wayland sessions, uses native rendering
- X11 fallback for legacy systems
- XDG Desktop Portal for file dialogs

### Detection
```cpp
QString platform = QApplication::platformName();
// Returns: "wayland" or "xcb" or "windows"
```

### Specific Considerations
- Avoid Qt::FramelessWindowHint (limited Wayland support)
- Use XDG Desktop Portal for file operations
- Test on GNOME Wayland and KDE Wayland

---

## 8. Installation Methods

### Terminal Installation (Snap/AppImage)
```bash
# Snap
sudo snap install cpu-reader

# AppImage
./cpu-reader-1.0-x86_64.AppImage

# Direct DEB
sudo apt install ./cpu-reader_0.2.0_amd64.deb

# From repository
sudo apt-add-repository ppa:mnl-nox/cpu-reader
sudo apt update
sudo apt install cpu-reader
```

### Desktop Installation
- Application appears in system launcher
- Double-click .deb file to install via GUI
- Registered in system menu

### Package Manager
- Debian/Ubuntu: .deb package
- Fedora/RHEL: .rpm package
- Arch: PKGBUILD
- Snap: Snap package
- Flatpak: Flatpak manifest

---

## 9. System Integration

### Desktop Entry (.desktop file)
```ini
[Desktop Entry]
Name=CPU Reader
Comment=Real-time CPU monitoring dashboard
Exec=cpu-reader
Icon=cpu-reader
Categories=Utilities;SystemMonitor;
Type=Application
Terminal=false
```

### Systemd Service (Optional)
```ini
[Unit]
Description=CPU Reader Daemon
After=network.target

[Service]
Type=simple
Exec=/usr/bin/cpu-reader --daemon
Restart=always
User=_cpu-reader
```

### Desktop Notifications
- Use D-Bus notifications
- Alert on high CPU/temperature
- Show in system notification center

---

## 10. Performance Requirements

- **GUI Response Time:** <100ms for user actions
- **Metrics Update:** 1s minimum interval (configurable)
- **Memory Usage:** <50MB for GUI application
- **CPU Overhead:** <2% when collecting metrics
- **Chart Rendering:** 60 FPS with 1h of data

---

## 11. Security Considerations

### Permissions
- Read-only access to /proc and /sys
- No privileged operations required
- Can run as regular user
- Optional: systemd service as dedicated user

### Data Privacy
- Historical data stored locally only
- No network communication by default
- Export to user-selected locations

### Binary Security
- Code signing for .deb packages
- GPG signature on releases
- SBOM (Software Bill of Materials) provided

---

## 12. Future Extensions

### Phase 2 (v0.3.0)
- Network CPU monitoring (remote systems)
- Web-based dashboard (Electron/React)
- REST API for metrics access

### Phase 3 (v0.4.0)
- Multi-system monitoring
- Alerting system (SMTP, Webhooks)
- Machine learning anomaly detection

### Phase 4 (v1.0.0)
- Cloud storage integration
- Advanced analytics
- Team collaboration features

---

## 13. Deployment Checklist

- [ ] Qt6 dependencies defined
- [ ] CMake build system configured
- [ ] GUI components implemented and tested
- [ ] System tray integration working
- [ ] Charts rendering smoothly
- [ ] Data export functionality
- [ ] Debian packaging configured
- [ ] .desktop file created
- [ ] Icons in multiple resolutions
- [ ] Documentation complete
- [ ] Cross-platform testing done
- [ ] Wayland testing on GNOME/KDE
- [ ] Performance benchmarks passed

---

## 14. References

- Qt 6 Documentation: https://doc.qt.io/qt-6/
- Wayland Documentation: https://wayland.freedesktop.org/
- Debian Packaging: https://www.debian.org/doc/manuals/debian-policy/
- FreeDesktop.org Desktop Entry Spec: https://specifications.freedesktop.org/desktop-entry-spec/

---

**Next Step:** Proceed to GUI-IMPLEMENTATION.md for detailed coding guidelines
