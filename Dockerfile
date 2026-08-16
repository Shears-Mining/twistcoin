# syntax=docker/dockerfile:1
FROM debian:bookworm-slim AS build

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        cmake ninja-build build-essential \
        libboost-dev libevent-dev libsqlite3-dev pkg-config libzmq3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -B build -DBUILD_GUI=OFF -DWITH_ZMQ=ON -DENABLE_WALLET=OFF -DBUILD_TESTS=OFF -DENABLE_IPC=OFF -G Ninja \
    && cmake --build build -j"$(nproc)"

FROM debian:bookworm-slim

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        libevent-core-2.1-7 libevent-extra-2.1-7 libevent-pthreads-2.1-7 libsqlite3-0 libzmq5 \
    && rm -rf /var/lib/apt/lists/* \
    && useradd --system --create-home --home-dir /data --shell /usr/sbin/nologin twistcoin

COPY --from=build /src/build/bin/twistcoind /usr/local/bin/
COPY --from=build /src/build/bin/twistcoin-cli /usr/local/bin/

USER twistcoin
VOLUME /data
WORKDIR /data

# 8433 = mainnet P2P (public); 8432 = mainnet RPC (internal only - don't publish this port)
EXPOSE 8433 8432

ENTRYPOINT ["twistcoind"]
CMD ["-datadir=/data", "-conf=/data/twistcoin.conf", "-printtoconsole"]
