FROM ubuntu:16.04
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get -y update \
    && apt-get -y install curl \
    && printf 'deb http://apt.postgresql.org/pub/repos/apt/ xenial-pgdg main\n' > /etc/apt/sources.list.d/postgres.list \
    && curl https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add - \
    && apt-get -y update \
    && apt-get -y install \
        cmake                        \
        postgresql-server-dev-9.6    \
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
