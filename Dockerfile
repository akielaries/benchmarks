FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -qy \
    && apt-get upgrade -qy \
    && apt-get install -y \
    bridge-utils \
    g++-12 \
    linux-perf \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /benchmarks

COPY . /benchmarks
