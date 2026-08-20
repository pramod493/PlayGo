FROM ubuntu:24.04 AS build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    libboost-filesystem-dev \
    libopencv-dev \
    libtbb-dev \
    pkg-config \
    qt5-qmake \
    qtbase5-dev \
    qtmultimedia5-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN mkdir build && cd build \
    && qmake ../playgo.pro CONFIG+=release \
    && make -j"$(nproc)"

FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    libboost-filesystem1.83.0 \
    libgl1 \
    libopencv-core406 \
    libopencv-imgcodecs406 \
    libopencv-imgproc406 \
    libopencv-photo406 \
    libqt5core5t64 \
    libqt5gui5t64 \
    libqt5multimedia5 \
    libqt5network5t64 \
    libqt5printsupport5t64 \
    libqt5sql5t64 \
    libqt5widgets5t64 \
    libtbb12 \
    xauth \
    xdotool \
    xvfb \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /src/build/exec/PlayGo /opt/playgo/PlayGo
COPY --from=build /src/images /opt/playgo/images

WORKDIR /opt/playgo
ENTRYPOINT ["/opt/playgo/PlayGo"]
