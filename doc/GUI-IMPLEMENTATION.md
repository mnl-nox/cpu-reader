# CPU Reader - GUI Implementation Guide

**Version:** 1.0  
**Date:** 2026-09-04  
**Phase:** v0.2.0 Implementation Plan

---

## 1. Build System Setup

### 1.1 CMakeLists.txt (Root)

```cmake
cmake_minimum_required(VERSION 3.24)
project(cpu-reader VERSION 0.2.0 LANGUAGES C CXX)

# C99 for library
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

# C++17 for GUI
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Find Qt6
find_package(Qt6 COMPONENTS
    Core
    Gui
    Widgets
    Charts
    DBus
    Svg
    REQUIRED
)

# Add library subdirectory
add_subdirectory(src)

# Add GUI subdirectory
add_subdirectory(gui)

# Add tests
enable_testing()
add_subdirectory(tests)

# Install targets
install(TARGETS cpu-reader
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
```

### 1.2 CMakeLists.txt (src/ - C Library)

```cmake
# Build libcpu as static library
add_library(cpu STATIC
    cpu.c
    cpu_info.c
    cpu_usage.c
)

target_include_directories(cpu
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_compile_options(cpu PRIVATE
    -std=c99
    -Wall -Wextra -Wpedantic
)

# For x86_64, enable assembly optimization
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    # Assembly code is already inline in cpu_usage.c
    target_compile_definitions(cpu PRIVATE HAVE_X86_64_ASM=1)
endif()

# Install library and headers
install(TARGETS cpu
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include/cpu.h
    DESTINATION include
)
```

### 1.3 CMakeLists.txt (gui/ - Qt Application)

```cmake
# Qt6 GUI Application
add_executable(cpu-reader
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/dashboard.cpp
    src/dashboard.h
    src/cpuchart.cpp
    src/cpuchart.h
    src/systemtray.cpp
    src/systemtray.h
    src/metrics_collector.cpp
    src/metrics_collector.h
    src/report_generator.cpp
    src/report_generator.h
    src/preferences.cpp
    src/preferences.h
    resources/resources.qrc
)

target_link_libraries(cpu-reader
    PRIVATE
    cpu
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts
    Qt6::DBus
    Qt6::Svg
)

target_include_directories(cpu-reader
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Wayland support
target_compile_definitions(cpu-reader PRIVATE
    QT_WAYLAND_ENABLED=1
)

# Install executable
install(TARGETS cpu-reader
    RUNTIME DESTINATION bin
)

# Install desktop file
install(FILES cpu-reader.desktop
    DESTINATION share/applications
)

# Install icons
install(DIRECTORY resources/icons/
    DESTINATION share/icons/hicolor
)
```

---

## 2. Core GUI Classes

### 2.1 main.cpp

```cpp
#include <QApplication>
#include "mainwindow.h"
#include <QSplashScreen>
#include <QPixmap>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application metadata
    QApplication::setApplicationName("CPU Reader");
    QApplication::setApplicationVersion("0.2.0");
    QApplication::setApplicationOrganizationName("CPU Reader Project");

    // Show splash screen
    QPixmap pixmap(":/icons/cpu-reader-256x256.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    splash.showMessage("Loading CPU Reader...",
                       Qt::AlignBottom | Qt::AlignCenter,
                       Qt::white);

    // Create and show main window
    MainWindow window;
    window.show();

    splash.finish(&window);

    return app.exec();
}
```

### 2.2 mainwindow.h

```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class Dashboard;
class SystemTray;
class MetricsCollector;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onShowHide();
    void onPreferences();
    void onExportMetrics();
    void onAbout();
    void onToggleMonitoring(bool enabled);

private:
    void createUI();
    void createMenuBar();
    void createConnections();
    void loadSettings();
    void saveSettings();

    std::unique_ptr<Dashboard> dashboard;
    std::unique_ptr<SystemTray> trayIcon;
    std::unique_ptr<MetricsCollector> metricsCollector;
};

#endif
```

### 2.3 mainwindow.cpp

```cpp
#include "mainwindow.h"
#include "dashboard.h"
#include "systemtray.h"
#include "metrics_collector.h"
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QMessageBox>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("CPU Reader - System Monitor");
    setWindowIcon(QIcon(":/icons/cpu-reader.svg"));
    setGeometry(100, 100, 1200, 700);

    createUI();
    createMenuBar();
    createConnections();
    loadSettings();

    // Start metrics collection in separate thread
    metricsCollector = std::make_unique<MetricsCollector>();
    QThread *thread = new QThread(this);
    metricsCollector->moveToThread(thread);

    connect(thread, &QThread::started,
            metricsCollector.get(), &MetricsCollector::start);
    connect(metricsCollector.get(), &MetricsCollector::metricsUpdated,
            dashboard.get(), &Dashboard::updateMetrics);
    connect(this, &QMainWindow::destroyed,
            thread, &QThread::quit);

    thread->start();
}

void MainWindow::createUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    dashboard = std::make_unique<Dashboard>();
    layout->addWidget(dashboard.get());

    setCentralWidget(centralWidget);
}

void MainWindow::createMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu("File");

    QAction *exportAction = fileMenu->addAction("Export Metrics");
    connect(exportAction, &QAction::triggered,
            this, &MainWindow::onExportMetrics);

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered,
            this, &QWidget::close);

    QMenu *viewMenu = menuBar()->addMenu("View");

    QAction *toggleAction = viewMenu->addAction("Toggle Monitoring");
    toggleAction->setCheckable(true);
    toggleAction->setChecked(true);
    connect(toggleAction, &QAction::triggered,
            this, &MainWindow::onToggleMonitoring);

    QMenu *helpMenu = menuBar()->addMenu("Help");

    QAction *aboutAction = helpMenu->addAction("About");
    connect(aboutAction, &QAction::triggered,
            this, &MainWindow::onAbout);
}

void MainWindow::onExportMetrics()
{
    QString filename = QFileDialog::getSaveFileName(this,
        "Export Metrics", "", "CSV Files (*.csv);;JSON Files (*.json)");

    if (!filename.isEmpty()) {
        // Call report generator
        dashboard->exportMetrics(filename);
    }
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About CPU Reader",
        "CPU Reader v0.2.0\n\n"
        "Real-time CPU monitoring dashboard\n\n"
        "MIT License - https://github.com/mnl-nox/cpu-reader");
}

MainWindow::~MainWindow() = default;
```

### 2.4 dashboard.h

```cpp
#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QChart>
#include <QLineSeries>
#include <memory>
#include <vector>
#include <deque>

struct CpuMetrics {
    qint64 timestamp;
    double usage;
    double temperature;
    double clockSpeed;
    int activeProcesses;
};

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);

public slots:
    void updateMetrics(const CpuMetrics &metrics);
    void exportMetrics(const QString &filename);

private:
    void createCharts();
    void updateChartData(const CpuMetrics &metrics);

    QChart *usageChart;
    QChart *temperatureChart;
    QtCharts::QLineSeries *usageSeries;
    QtCharts::QLineSeries *temperatureSeries;

    std::deque<CpuMetrics> metricsHistory;
    const size_t maxHistorySize = 3600; // 1 hour at 1s interval
};

#endif
```

### 2.5 metrics_collector.h

```cpp
#ifndef METRICS_COLLECTOR_H
#define METRICS_COLLECTOR_H

#include <QObject>
#include <QTimer>
#include <memory>

struct CpuMetrics;

class MetricsCollector : public QObject
{
    Q_OBJECT

public:
    explicit MetricsCollector(QObject *parent = nullptr);
    ~MetricsCollector();

public slots:
    void start();
    void stop();
    void setInterval(int milliseconds);

signals:
    void metricsUpdated(const CpuMetrics &metrics);
    void errorOccurred(const QString &error);

private slots:
    void collectMetrics();

private:
    QTimer *timer;
    int intervalMs;
};

#endif
```

### 2.6 metrics_collector.cpp

```cpp
#include "metrics_collector.h"
#include "cpu.h"
#include <QTimer>
#include <QDebug>
#include <chrono>

struct CpuMetrics {
    qint64 timestamp;
    double usage;
    double temperature;
    double clockSpeed;
    int activeProcesses;
};

MetricsCollector::MetricsCollector(QObject *parent)
    : QObject(parent), timer(nullptr), intervalMs(1000)
{
}

MetricsCollector::~MetricsCollector()
{
    stop();
    cpu_cleanup();
}

void MetricsCollector::start()
{
    if (cpu_init() != 0) {
        emit errorOccurred("Failed to initialize CPU Reader library");
        return;
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MetricsCollector::collectMetrics);
    timer->start(intervalMs);

    qDebug() << "MetricsCollector started with interval:" << intervalMs << "ms";
}

void MetricsCollector::stop()
{
    if (timer) {
        timer->stop();
        delete timer;
        timer = nullptr;
    }
}

void MetricsCollector::setInterval(int milliseconds)
{
    intervalMs = milliseconds;
    if (timer && timer->isActive()) {
        timer->setInterval(milliseconds);
    }
}

void MetricsCollector::collectMetrics()
{
    CpuMetrics metrics;
    metrics.timestamp = QDateTime::currentMSecsSinceEpoch();

    // Get CPU usage
    metrics.usage = cpu_get_usage();

    // Get temperature (returns -1.0f if unavailable)
    metrics.temperature = cpu_get_temperature();

    // Get clock speed (returns -1.0f if unavailable)
    metrics.clockSpeed = cpu_get_clock_speed();

    // Get active processes
    metrics.activeProcesses = cpu_get_active_processes();

    if (metrics.usage < 0 && metrics.usage != -1.0f) {
        emit errorOccurred("Failed to collect metrics");
        return;
    }

    emit metricsUpdated(metrics);
}
```

---

## 3. System Tray Integration

### 3.1 systemtray.h

```cpp
#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <memory>

class MainWindow;

class SystemTray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTray(MainWindow *parent = nullptr);
    void updateMetrics(double usage, double temperature);

private:
    void createMenu();
    MainWindow *mainWindow;
    QMenu *trayMenu;
    QAction *usageAction;
    QAction *temperatureAction;
};

#endif
```

### 3.2 systemtray.cpp

```cpp
#include "systemtray.h"
#include "mainwindow.h"
#include <QMenu>
#include <QAction>
#include <QApplication>

SystemTray::SystemTray(MainWindow *parent)
    : QSystemTrayIcon(parent), mainWindow(parent)
{
    setIcon(QIcon(":/icons/cpu-reader.svg"));
    createMenu();
    setToolTip("CPU Reader - System Monitor");

    connect(this, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            mainWindow->setVisible(!mainWindow->isVisible());
        }
    });

    show();
}

void SystemTray::createMenu()
{
    trayMenu = new QMenu(mainWindow);

    usageAction = trayMenu->addAction("CPU: --");
    temperatureAction = trayMenu->addAction("Temp: --°C");

    trayMenu->addSeparator();

    QAction *showAction = trayMenu->addAction("Show/Hide");
    connect(showAction, &QAction::triggered, [this]() {
        mainWindow->setVisible(!mainWindow->isVisible());
    });

    QAction *settingsAction = trayMenu->addAction("Preferences");
    connect(settingsAction, &QAction::triggered,
            mainWindow, [this]() { mainWindow->showSettings(); });

    trayMenu->addSeparator();

    QAction *exitAction = trayMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, QApplication::quit);

    setContextMenu(trayMenu);
}

void SystemTray::updateMetrics(double usage, double temperature)
{
    usageAction->setText(QString("CPU: %1%").arg(usage, 0, 'f', 1));

    if (temperature > 0) {
        temperatureAction->setText(QString("Temp: %1°C").arg(temperature, 0, 'f', 1));
    } else {
        temperatureAction->setText("Temp: N/A");
    }

    setToolTip(QString("CPU Reader\nUsage: %1%\nTemperature: %2°C")
               .arg(usage, 0, 'f', 1)
               .arg(temperature > 0 ? QString::number(temperature, 'f', 1) : "N/A"));
}
```

---

## 4. Qt Resources File

### 4.1 resources/resources.qrc

```xml
<RCC>
    <qresource prefix="/">
        <file>icons/cpu-reader.svg</file>
        <file>icons/cpu-reader-16x16.png</file>
        <file>icons/cpu-reader-64x64.png</file>
        <file>icons/cpu-reader-256x256.png</file>
    </qresource>
</RCC>
```

---

## 5. Desktop Entry File

### 5.1 gui/cpu-reader.desktop

```ini
[Desktop Entry]
Type=Application
Name=CPU Reader
Comment=Real-time CPU monitoring dashboard
Icon=cpu-reader
Exec=cpu-reader %F
Terminal=false
Categories=System;Utilities;Monitor;
StartupNotify=true
Keywords=cpu;monitor;system;performance;
```

---

## 6. Compilation Commands

### Quick Build

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Debug Build

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
./gui/cpu-reader
```

### Release Build

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

---

## 7. Key Development Tasks

- [ ] Set up CMake build system
- [ ] Create main window with menu bar
- [ ] Implement dashboard with layouts
- [ ] Create CPU usage chart using Qt Charts
- [ ] Implement metrics collector thread
- [ ] Add system tray integration
- [ ] Create preferences/settings dialog
- [ ] Implement metrics export (CSV/JSON/PDF)
- [ ] Create desktop entry and icons
- [ ] Test Wayland compatibility
- [ ] Create .deb packaging
- [ ] Write integration tests

---

**Next Step:** Proceed to DEPLOYMENT.md for packaging and distribution
