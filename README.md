# PlayGo

PlayGo is a Qt desktop application for sketching and simulating 2D physical
models. The supported Linux build uses Qt 5.15, OpenCV 4, GCC, Boost, and
oneTBB on Ubuntu 24.04.

## Recommended: build and run with Docker

Docker contains the complete compiler and runtime dependency set, so no Qt,
OpenCV, Boost, or TBB packages need to be installed on the host.

```bash
docker build -t playgo:latest .
./scripts/test-docker.sh
./scripts/run-docker.sh
```

The run script requires a Linux graphical session with `DISPLAY` set and
access to the X11/XWayland socket. Application data is persisted in
`~/.local/share/playgo` by default; override it with `PLAYGO_DATA_DIR`.
If the X server rejects the connection, authorize the current local user:

```bash
xhost +si:localuser:"$(id -un)"
```

See [docs/BUILDING.md](docs/BUILDING.md) for native build instructions and
[docs/MODERNIZATION.md](docs/MODERNIZATION.md) for the upgrade details.

## Native build with Conda

On a machine with Conda, no administrator access is required:

```bash
./scripts/install-local-deps.sh
./scripts/build-local.sh
./scripts/run-local.sh
```
