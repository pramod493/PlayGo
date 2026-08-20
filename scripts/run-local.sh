#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
conda_prefix="${CONDA_PREFIX:-$(conda info --base)}"
data_dir="${PLAYGO_DATA_DIR:-${HOME}/.local/share/playgo-native}"

export CONDA_PREFIX="$conda_prefix"
export PATH="$CONDA_PREFIX/bin:$PATH"
export LD_LIBRARY_PATH="$CONDA_PREFIX/lib:${LD_LIBRARY_PATH:-}"
export QT_PLUGIN_PATH="$CONDA_PREFIX/plugins"

mkdir -p "$data_dir"
cd "$data_dir"
exec "$repo_root/build-native/exec/PlayGo"
