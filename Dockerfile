FROM debian:trixie-slim

ENV DEBIAN_FRONTEND=noninteractive
ENV PYTHONDONTWRITEBYTECODE=1

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        bash \
        ca-certificates \
        clang \
        cmake \
        git \
        make \
        ninja-build \
        python3 \
        python3-pip \
        sdcc-ucsim \
    && python3 -m pip install --break-system-packages --no-cache-dir pyelftools \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
