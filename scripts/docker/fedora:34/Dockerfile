FROM fedora:34

RUN dnf -y clean all                  \
        && dnf -y install             \
                cmake                 \
                pkgconf               \
                make                  \
                gcc-c++               \
                git                   \
                boost-devel           \
                libidn-devel          \
                mpdecimal-devel       \
                libssh-devel          \
                openssl-devel

RUN rpm -Uvh https://download.postgresql.org/pub/repos/yum/reporpms/F-34-x86_64/pgdg-fedora-repo-latest.noarch.rpm \
        && dnf -y install postgresql96-devel
