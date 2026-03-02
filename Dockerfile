# Dockerfile for DIT-Transfer-Tools cross-platform build

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    qt6-base-dev \
    cmake \
    build-essential \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN mkdir build && cd build && cmake .. && make -j$(nproc)

CMD ["./build/DIT-Transfer-Tools"]