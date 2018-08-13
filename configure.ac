AC_PREREQ([2.59])
AC_INIT([libfred], [0.0.1], [noreply@nic.cz])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects tar-ustar filename-length-max=220])
m4_pattern_allow([^AC_MSG_])
AC_DISABLE_STATIC
AC_ENABLE_SHARED
AM_PROG_AR
LT_INIT
AC_PROG_LIBTOOL(libtool)
AC_PROG_CC
AC_CONFIG_FILES([Makefile])
AC_DEFINE_UNQUOTED([HAVE_LOGGER], [1], [compile libraries with logging messages])

AC_LANG(C++)

: ${CFLAGS=""}   AC_PROG_CC
: ${CXXFLAGS=""} AC_PROG_CXX

PKG_PROG_PKG_CONFIG

AX_CHECK_COMPILE_FLAG([-std=c++14],
                      [CXXFLAGS="$CXXFLAGS -std=c++14"],
                      [AC_MSG_ERROR([This project requires -std=c++14 support])])

AX_BOOST_BASE
AX_BOOST_DATE_TIME
AX_BOOST_REGEX
AX_BOOST_THREAD 
AX_BOOST_SYSTEM

# Checks for PostgreSQL headers and libs
AX_LIB_POSTGRESQL([8.0])

AC_CHECK_FUNCS([fork],,[AC_MSG_ERROR([Function fork not found])])
AC_CHECK_FUNCS([waitpid],,[AC_MSG_ERROR([Function waitpid not found])])
AC_CHECK_FUNCS([kill],,[AC_MSG_ERROR([Function kill not found])])

AC_CHECK_HEADER([unistd.h],,[AC_MSG_ERROR([Header unistd.h not found])])

AC_CHECK_HEADER([boost/optional.hpp],,[AC_MSG_ERROR([Header boost/optional.hpp not found])])

AC_OUTPUT
