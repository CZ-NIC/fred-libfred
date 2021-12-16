FROM fedora:35

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
                openssl-devel

RUN rpm -Uvh https://download.postgresql.org/pub/repos/yum/reporpms/F-35-x86_64/pgdg-fedora-repo-latest.noarch.rpm \
        && dnf -y install postgresql13-devel
