# CPU Reader v0.2.0 - Getting Started with GUI Development

**Date:** 2026-09-04  
**Phase:** Week 1 - Setup & Preparation  
**Duration:** 5 days (Sep 4-8)

---

## Overview

This document guides the first week of v0.2.0 development: environment setup, dependency installation, and project structure preparation.

---

## Day 1 (Sep 4) - Planning & Environment Setup

### Morning: Team Kickoff

- [ ] Review GUI-SUMMARY.md (this team)
- [ ] Review GUI-ARCHITECTURE.md (technical)
- [ ] Review ROADMAP-GUI.md (timeline)
- [ ] Q&A session (30 min)

### Afternoon: Environment Setup

#### Install Qt6 Development Tools (Ubuntu/Debian)

```bash
# Update package lists
sudo apt update

# Install Qt6 and dependencies
sudo apt install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-charts-dev \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    git \
    qt-creator

# Verify installation
qmake --version
cmake --version
```

#### Install Qt6 Development Tools (Fedora/RHEL)

```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    qt6-qtbase-devel \
    qt6-qtcharts-devel \
    qt6-qttools-devel \
    git

# Verify
qmake --version
cmake --version
```

---

## Day 2 (Sep 5) - Git Setup & Project Structure

### Create Feature Branch

```bash
cd /home/nox/cpu-reader
git checkout -b feature/v0.2-gui-foundation
git branch -vv
```

### Create Directory Structure

```bash
# Create gui subdirectory
mkdir -p gui/src
mkdir -p gui/resources/icons
mkdir -p gui/resources/ui

# Create packaging subdirectory
mkdir -p packaging/debian
mkdir -p packaging/docker
mkdir -p packaging/scripts

# Create scripts directory
mkdir -p scripts
```

---

## Day 3 (Sep 6) - CMake Configuration

### Create Root CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.24)
project(cpu-reader VERSION 0.2.0 LANGUAGES C CXX)

# Standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Charts
    Svg
)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(gui)
add_subdirectory(tests)
```

### Test Build System

```bash
cd /home/nox/cpu-reader
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --target help
```

---

## Day 4 (Sep 7) - Core GUI Classes

### Create Skeleton Files

**gui/src/main.cpp**:

```cpp
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    std::cout << "CPU Reader v0.2.0 GUI" << std::endl;
    return app.exec();
}
```

**gui/src/mainwindow.h**:

```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    void createUI();
    void createMenuBar();
};

#endif
```

**gui/src/mainwindow.cpp**:

```cpp
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("CPU Reader - System Monitor");
    setGeometry(100, 100, 1200, 700);

    createUI();
    createMenuBar();
}

void MainWindow::createUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QLabel *label = new QLabel("CPU Reader Dashboard");
    layout->addWidget(label);

    setCentralWidget(centralWidget);
}

void MainWindow::createMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("Exit", this, &QWidget::close);
}
```

### Create gui/CMakeLists.txt

```cmake
add_executable(cpu-reader
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
)

target_link_libraries(cpu-reader
    PRIVATE
    cpu
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts
    Qt6::Svg
)

target_include_directories(cpu-reader
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

install(TARGETS cpu-reader RUNTIME DESTINATION bin)
```

### Build and Test

```bash
cd build
cmake ..
cmake --build .
./gui/cpu-reader
```

---

## Day 5 (Sep 8) - Finalize & Commit

### Update Documentation Links

Update README.md and doc/README.md to include new docs.

### Prepare Commit

All new documentation files created:

- doc/GUI-ARCHITECTURE.md (900+ lines)
- doc/GUI-IMPLEMENTATION.md (600+ lines)
- doc/GUI-QUICKSTART.md (500+ lines)
- doc/DEPLOYMENT.md (800+ lines)
- doc/ROADMAP-GUI.md (700+ lines)
- doc/GUI-SUMMARY.md (500+ lines)
- doc/GUI-GETTING-STARTED.md (this file)

Project structure prepared:

- gui/ directory with src/resources subdirs
- packaging/ directory for distributions
- scripts/ directory for build/package scripts
- CMakeLists.txt files configured
- Skeleton GUI application ready

---

## Next Steps (Week 2)

**Monday-Tuesday (Sep 11-12)**: Validation

- Run sanitizer tests
- Multi-platform validation
- Update VALIDATION.md

**Wednesday-Friday (Sep 13-15)**: Core Implementation

- Dashboard widget
- CPU charts
- Metrics collector thread
- System tray integration

---

## Commands Checklist

```bash
# Day 1: Verify environment
qmake --version
cmake --version

# Day 2: Create structure
mkdir -p gui/src gui/resources/{icons,ui}
mkdir -p packaging/{debian,docker,scripts} scripts

# Day 3: CMake build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Day 4: Compile GUI
cmake --build .
./gui/cpu-reader

# Day 5: Commit
git add -A
git commit -m "feat(v0.2.0): gui foundation and planning"
```

---

**Week 1 Completion**: ✓ Environment ready, structure created, skeleton builds
