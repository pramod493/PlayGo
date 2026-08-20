# Linux build modernization

## Baseline

The supported baseline is Ubuntu 24.04 with:

| Component | Version supplied by Ubuntu 24.04 |
| --- | --- |
| GCC | 13 |
| Qt | 5.15.13 |
| OpenCV | 4.6 |
| Boost | 1.83 |
| oneTBB | 2021.11 |
| C++ standard | C++17 |

Qt 5.15 is the newest low-risk Qt line for this codebase. Moving directly to
Qt 6 would also require replacing or upgrading the bundled qCustomPlot code
and porting touch, tablet, and other removed Qt APIs. OpenCV and the compiler
toolchain are current distribution-supported versions.

## Changes made

- Added a multi-stage Docker build that compiles and packages PlayGo without
  modifying the host system.
- Added Docker launch and headless smoke-test scripts.
- Verifies that the GUI creates its main window and can write application
  data during the smoke test.
- Changed all qmake projects to C++17.
- Replaced hard-coded OpenCV 3 libraries with `pkg-config` discovery of
  `opencv4`.
- Removed the obsolete `opencv_ts` dependency and legacy Windows-only
  assumptions from the Linux link path.
- Updated removed OpenCV constants to the OpenCV 4 enum names.
- Ported legacy TBB scheduler and timing use to oneTBB-compatible APIs and
  standard C++ timing.
- Corrected static-library dependency order for modern GNU linkers.
- Fixed strict modern-compiler errors for pointer and Qt flag return values.
- Moved logs from the executable directory to Qt's writable application-data
  directory so the container can run as the invoking desktop user.
- Runs the GUI with the invoking user's UID and a persistent writable data
  directory rather than granting the container root access.
- Excluded generated build and library output from version control.

The repository continues to bundle its application-side dependencies:
Box2D, poly2tri, qCustomPlot, QsLog, and the P-Dollar recognizer. Large,
security-maintained platform dependencies (Qt, OpenCV, Boost, and oneTBB)
come from Ubuntu packages inside the Docker image instead of checked-in
binaries. This keeps host installation at zero while allowing security
updates through image rebuilds.

## Rebuilding with updates

Rebuild without Docker's package cache to pick up patched Ubuntu packages:

```bash
docker build --pull --no-cache -t playgo:latest .
./scripts/test-docker.sh
```
