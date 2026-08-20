# PlayGo project context

## Current state

PlayGo is a legacy Qt/Box2D/OpenCV desktop application modernized for
Ubuntu 24.04. The maintained Linux build uses:

- C++17 with GCC 13 or newer
- Qt 5.15 (Qt 6 is not yet supported)
- OpenCV 4 discovered through `pkg-config`
- Boost.Filesystem and Boost.System
- oneTBB
- Bundled Box2D, poly2tri, qCustomPlot, QsLog, and P-Dollar sources

The Linux modernization was completed on branch
`pramod493-modernize-linux-build`:

- `4735e15` modernized the build and added native and Docker workflows.
- `0132982` enabled downward gravity for simulation.

## Build and run

Prefer the native Conda workflow on this machine. Dependencies are installed
in `/home/pramod/anaconda3`; no sudo or Docker is required.

```bash
./scripts/install-local-deps.sh  # Only needed to install/update dependencies
./scripts/build-local.sh
./scripts/run-local.sh
```

The release executable is generated at `build-native/exec/PlayGo`.
Application data and logs default to `~/.local/share/playgo-native`.

The reproducible container workflow is also available:

```bash
docker build -t playgo:latest .
./scripts/test-docker.sh
./scripts/run-docker.sh
```

See `docs/BUILDING.md` for full setup instructions and
`docs/MODERNIZATION.md` for the upgrade rationale and changes.

## Physics behavior

Gravity is configured in `CDI::PhysicsSettings` in
`Core/physicsmanager.h` as `Point2D(0.0f, 10.0f)`, which is downward in the
scene coordinate system. It is enabled by default. `Core/page.cpp` preserves
that default when creating a page, `PhysicsManager::init()` applies the
enabled state to Box2D, and `PlayGoController::startSimulation()` explicitly
enables gravity before starting the timer.

Interactive editing modes may temporarily disable gravity through
`PlayGoController::onModeChange()`; full simulation mode re-enables it.

## Validation expectations

After build-related or physics changes:

```bash
./scripts/build-local.sh
timeout 10s ./scripts/run-local.sh
```

A timeout exit after the application remains open is expected for the manual
GUI smoke test. Do not commit generated `build-native/`, `build/`, or `lib/`
contents.

## Known constraints

- The old qmake project remains in use; do not assume CMake.
- The bundled qCustomPlot and legacy Qt input APIs currently require Qt 5.
- Existing compiler warnings in bundled/legacy code are not build failures.
- Preserve repository-bundled application libraries unless deliberately
  planning a broader dependency migration.
