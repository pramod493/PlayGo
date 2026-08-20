#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
conda_prefix="${CONDA_PREFIX:-$(conda info --base)}"

export CONDA_PREFIX="$conda_prefix"
export PATH="$CONDA_PREFIX/bin:$PATH"
export PKG_CONFIG_PATH="$CONDA_PREFIX/lib/pkgconfig:$CONDA_PREFIX/share/pkgconfig"

mkdir -p "$repo_root/build-native"
cd "$repo_root/build-native"
qmake ../playgo.pro CONFIG+=release
make -j"$(nproc)"
