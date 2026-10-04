FROM debian:bookworm-slim

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get upgrade -y \
    && apt-get install --no-install-recommends -y \
       build-essential \
       clang \
       cppcheck \
       libncurses-dev \
       ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY . .

RUN useradd --create-home --uid 10001 tester \
    && chown -R tester:tester /workspace

USER tester

CMD ["make", "test"]
