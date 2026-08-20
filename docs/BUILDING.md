# Building PlayGo

## Container build (recommended)

Requirements:

- Docker Engine with BuildKit
- A Linux X11 or XWayland desktop only when running the GUI

Build the deployable image:

```bash
docker build -t playgo:latest .
```

Run the automated headless startup check:

```bash
./scripts/test-docker.sh
```

Launch the GUI:

```bash
./scripts/run-docker.sh
```

Set `PLAYGO_IMAGE` when using a different image tag. The image is based on
Ubuntu 24.04 and contains all runtime libraries; the host needs only Docker.
The launch script mounts `~/.local/share/playgo` as the writable working
directory so saved models and logs survive container removal. Set
`PLAYGO_DATA_DIR` to use another host directory.

## Native Ubuntu 24.04 build

### Without administrator access

An existing Conda installation can provide the complete native toolchain
without Docker or system package changes:

```bash
./scripts/install-local-deps.sh
./scripts/build-local.sh
./scripts/run-local.sh
```

The dependency script installs Qt 5.15.2, current OpenCV 4, Boost, oneTBB,
OpenGL headers, and pkg-config into the active Conda environment. The build
script uses `CONDA_PREFIX` when an environment is active and otherwise uses
the Conda base environment. Application data is written to
`~/.local/share/playgo-native` unless `PLAYGO_DATA_DIR` is set.

### With Ubuntu packages

Native builds require these tools and development packages:

- GCC/G++ 13 or newer and GNU Make
- qmake from Qt 5.15
- Qt 5 Core, GUI, Widgets, Network, Multimedia, PrintSupport, and SQL
- OpenCV 4 development files and `opencv4.pc`
- Boost.Filesystem and Boost.System development files
- oneTBB development files
- pkg-config

Install the Ubuntu packages:

```bash
sudo apt update
sudo apt install \
  build-essential \
  libboost-filesystem-dev \
  libopencv-dev \
  libtbb-dev \
  pkg-config \
  qt5-qmake \
  qtbase5-dev \
  qtmultimedia5-dev
```

Build out of tree:

```bash
mkdir -p build
cd build
qmake ../playgo.pro CONFIG+=release
make -j"$(nproc)"
```

Run the resulting executable:

```bash
./exec/PlayGo
```

For a debug build, replace `CONFIG+=release` with `CONFIG+=debug`. Generated
static libraries are placed in `lib/`; build output is placed in `build/`.

## Other distributions

Use equivalent packages that provide Qt 5.15 development tools, OpenCV 4,
Boost.Filesystem, oneTBB, and pkg-config. Confirm OpenCV discovery before
building:

```bash
pkg-config --modversion opencv4
qmake -query QT_VERSION
```

Qt 6 is not currently supported. The application relies on Qt 5 input-event
and graphics APIs, and its bundled qCustomPlot release predates Qt 6.
