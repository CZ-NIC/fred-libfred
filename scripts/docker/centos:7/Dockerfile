FROM centos/devtoolset-7-toolchain-centos7
USER root

RUN yum -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm \
    && yum -y install https://download.postgresql.org/pub/repos/yum/9.6/redhat/rhel-7-x86_64/pgdg-redhat96-9.6-3.noarch.rpm \
    && yum -y clean all       \
    && yum -y install         \
        cmake3                \
        pkgconf               \
        make                  \
        gcc-c++               \
        git                   \
        postgresql96-devel    \
        boost-devel           \
        libidn-devel          \
        mpdecimal-devel       \
        libssh-devel          \
        openssl-devel         \
    && rm -rf /var/cache/yum

USER 1001
