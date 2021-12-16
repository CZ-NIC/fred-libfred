FROM fedora:rawhide

RUN dnf -y clean all                  \
        && dnf -y install             \
                findutils             \
                cmake                 \
                pkgconf               \
                make                  \
                gcc-c++               \
                git                   \
                boost-devel           \
                libidn-devel          \
                mpdecimal-devel       \
                libssh-devel          \
                openssl-devel         \
                postgresql-server-devel
