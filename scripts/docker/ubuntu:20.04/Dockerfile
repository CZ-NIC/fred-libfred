FROM ubuntu:20.04
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get -y update \
    && apt-get -y install --no-install-recommends curl gnupg2 ca-certificates \
    && printf 'deb http://apt.postgresql.org/pub/repos/apt/ focal-pgdg main\n' > /etc/apt/sources.list.d/postgres.list \
    && curl https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add -

RUN apt-get -y update \
    && apt-get -y install --no-install-recommends \
        ssh-client                   \
        cmake                        \
        postgresql-server-dev-13     \
        pkg-config                   \
        build-essential              \
        bash-completion              \
        git                          \
        libboost-date-time-dev       \
        libboost-program-options-dev \
        libboost-regex-dev           \
        libboost-filesystem-dev      \
        libboost-random-dev          \
        libboost-thread-dev          \
        libboost-system-dev          \
        libboost-serialization-dev   \
        libboost-test-dev            \
        libpq-dev                    \
        libmpdec-dev                 \
        libssh-dev                   \
        libidn11-dev                 \
    && apt-get -y clean              \
    && rm -rf /var/lib/apt/lists/*
