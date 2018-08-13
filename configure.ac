AC_PREREQ([2.59])
AC_INIT([libfred], [0.0.1], [noreply@nic.cz])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_ENABLE_SHARED
AM_PROG_AR
LT_INIT
AC_PROG_LIBTOOL(libtool)
AC_PROG_CC
AC_CONFIG_FILES([Makefile])

AC_LANG(C++)

: ${CFLAGS=""}   AC_PROG_CC
: ${CXXFLAGS=""} AC_PROG_CXX

AX_CHECK_COMPILE_FLAG([-std=c++14],
                      [CXXFLAGS="$CXXFLAGS -std=c++14"],
                      [AC_MSG_ERROR([This project requires -std=c++14 support])])

AX_BOOST_BASE
AX_BOOST_SYSTEM

AC_CHECK_FUNCS([fork],,[AC_MSG_ERROR([Function fork not found])])
AC_CHECK_FUNCS([waitpid],,[AC_MSG_ERROR([Function waitpid not found])])
AC_CHECK_FUNCS([kill],,[AC_MSG_ERROR([Function kill not found])])

AC_CHECK_HEADER([unistd.h],,[AC_MSG_ERROR([Header unistd.h not found])])

AC_CHECK_HEADER([boost/optional.hpp],,[AC_MSG_ERROR([Header boost/optional.hpp not found])])

AC_OUTPUT

