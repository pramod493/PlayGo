#!/usr/bin/env bash
set -euo pipefail

conda_bin="${CONDA_EXE:-$(command -v conda)}"

"$conda_bin" install -y -c conda-forge \
    boost-cpp \
    libgl-devel \
    opencv \
    pkg-config \
    qt-main=5.15.2 \
    tbb-devel
