#!/usr/bin/env bash
set -euo pipefail

image="${PLAYGO_IMAGE:-playgo:latest}"
data_dir="${PLAYGO_DATA_DIR:-${HOME}/.local/share/playgo}"

if [[ -z "${DISPLAY:-}" ]]; then
    echo "DISPLAY is not set; run this script from a graphical Linux session." >&2
    exit 1
fi

mkdir -p "$data_dir"

docker run --rm \
    --network host \
    --user "$(id -u):$(id -g)" \
    --env DISPLAY \
    --env HOME=/data \
    --env QT_X11_NO_MITSHM=1 \
    --volume /tmp/.X11-unix:/tmp/.X11-unix:rw \
    --volume "$data_dir":/data \
    --workdir /data \
    "$image"
