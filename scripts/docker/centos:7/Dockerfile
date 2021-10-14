FROM centos/devtoolset-7-toolchain-centos7
USER root

RUN yum -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm \
    && yum -y install https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm \
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
    && yum -y remove          \
        cmake                 \
    && rm -rf /var/cache/yum

RUN ln -s /usr/bin/cmake3 /usr/bin/cmake       \
    && ln -s /usr/bin/ccmake3 /usr/bin/ccmake  \
    && ln -s /usr/bin/ctest3 /usr/bin/ctest    \
    && ln -s /usr/bin/cpack3 /usr/bin/cpack
