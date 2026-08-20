#!/usr/bin/env bash
set -euo pipefail

image="${PLAYGO_IMAGE:-playgo:latest}"

docker run --rm \
    --user "$(id -u):$(id -g)" \
    --env HOME=/data \
    --tmpfs /data:uid="$(id -u)",gid="$(id -g)" \
    --workdir /data \
    --entrypoint xvfb-run \
    "$image" -a sh -ec '
        /opt/playgo/PlayGo &
        app_pid=$!
        trap "kill $app_pid 2>/dev/null || true" EXIT

        attempt=0
        while [ "$attempt" -lt 20 ]; do
            kill -0 "$app_pid" 2>/dev/null || {
                echo "PlayGo exited before creating its main window." >&2
                wait "$app_pid"
            }
            window_id="$(xdotool search --name "CDI Sketcher - PlayGo" 2>/dev/null | head -n 1 || true)"
            if [ -n "$window_id" ]; then
                xdotool getwindowname "$window_id"
                test -f /data/.local/share/Purdue/CDI_Sketcher/log.txt
                echo "PlayGo created its main window and writable application data."
                exit 0
            fi
            attempt=$((attempt + 1))
            sleep 0.5
        done

        echo "PlayGo did not create its main window within 10 seconds." >&2
        exit 1
    '
