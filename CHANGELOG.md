# Changelog - CPU Reader

## [Unreleased]

### Changed
- Reorganized the repository documentation to match the current file structure
- Added dedicated build and validation guides at the repository root
- Simplified the main README and documentation index to reference only existing files

## [v1.0.0-beta.1] - 2026-10-04

### Added
- Portable C validation target for architectures without x86_64 assembly
- Explicit Beta compatibility matrix for Linux x86_64, arm64 and armv7

### Changed
- Made ncurses optional for building and testing the core library
- Updated build, validation and API documentation for Beta usage

## [v0.1.1] - 2026-09-04

### Fixed
- Remove all emoji characters from documentation files for better compatibility and accessibility
- Fix spacing issues in markdown after emoji removal
- Improve document readability with clean formatting

### Improved
- Cleaned up BUILD.md documentation
- Cleaned up VALIDATION.md documentation
- Cleaned up PROGRESS.md documentation
- Cleaned up README.md documentation
- Cleaned up all doc/ subdirectory markdown files:
  - doc/CONSOLIDACAO-VALIDACAO.md
  - doc/README.md
  - doc/architecture-decision-records.md
  - doc/requisitos-funcionais.md
  - doc/requisitos-nao-funcionais.md
  - doc/roadmap-evolucao-semana.md
  - doc/software-requirements-specification.md

### Documentation
- Maintained all 24 requirements (12 RF + 12 RNF)
- Maintained all 13 ADRs (Architecture Decision Records)
- Maintained all 14 UML diagrams
- Maintained 8-week roadmap
- Maintained validation report for all requirements

### Status
- All 12 Functional Requirements (RF-001 to RF-012): VALIDATED
- 10/12 Non-Functional Requirements: VALIDATED
- Compilation: 0 warnings
- Tests: 100% passing
- Code quality: Maintained

## [v0.1.0] - 2026-09-04

### Added
- Initial consolidation of CPU Reader project documentation
- 11 comprehensive documentation files (7500+ lines):
  - Requirements specification (functional and non-functional)
  - Architecture decision records (13 formalized)
  - Software requirements specification (SRS)
  - Design document v2.0
  - UML diagrams (14 Mermaid diagrams)
  - 8-week evolution roadmap
  - Build instructions
  - Validation report
  - Progress tracking

- 24 requirements documented:
  - 12 Functional Requirements (RF-001 to RF-012)
  - 12 Non-Functional Requirements (RNF-001 to RNF-012)
  - 3 Future Requirements (RF-013 to RF-015)

- 13 Architecture Decision Records:
  - ADR-0001 to ADR-0013 (accepted and proposed)

- 14 UML Diagrams:
  - Class diagrams
  - Component diagrams
  - Sequence diagrams
  - State diagrams
  - Flow diagrams
  - Call graphs
  - Dependency hierarchies

- Build and validation tools:
  - Comprehensive build guide (BUILD.md)
  - Detailed validation report (VALIDATION.md)
  - Progress tracking (PROGRESS.md)

### Verified
- 12/12 Functional Requirements: VALIDATED
- 10/12 Non-Functional Requirements: VALIDATED (2 pending for Week 2)
- C99 compilation: 0 warnings
- Unit tests: 100% passing
- Monitor application: Working

---

## Release Notes

### v0.1.1 Highlights
- Maintained all technical content and specifications
- Ready for Week 2 validation phase (sanitizers and multi-platform testing)

### v0.1.0 Highlights
- Complete documentation consolidation for Semana 1
- All requirements formally specified
- Architectural decisions documented
- UML diagrams for all perspectives
- 8-week roadmap with daily tasks

---

## Schedule

- Semana 1 (2026-09-04): Documentation consolidation - COMPLETE
- Semana 2 (2026-09-11): Validation and testing (sanitizers, multi-platform)
- Semana 3 (2026-09-16): CI/CD setup
- Semana 4+ (2026-09-23): Features and releases

---

## Contributors

- Documentation: Team
- Implementation: Already completed (v0.1.0)
- Validation: Ongoing

---

## Next Steps

1. Week 2: Sanitizer testing and multi-distribution validation
2. Week 3: GitHub Actions CI/CD setup
3. Week 4: Feature RF-013 (per-core metrics) and v0.2.0 release
