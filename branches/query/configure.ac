# Process this file with autoconf to produce a configure script.
# $Id: configure.ac 246 2005-04-26 12:03:53Z ka0ttic $

AC_PREREQ(2.59)
AC_INIT(src/herdstat.cc)

# release versioning
VERSION_MAJOR=1
VERSION_MINOR=1
VERSION_MICRO=2
VERSION_SUFFIX=rc
VERSION_SUFFIX_VERSION=1
VERSION_FULL="$VERSION_MAJOR.$VERSION_MINOR.$VERSION_MICRO"

if ! test -z "$VERSION_SUFFIX" ; then
    VERSION_FULL="$VERSION_FULL"_"$VERSION_SUFFIX""$VERSION_SUFFIX_VERSION"
fi

AC_SUBST(VERSION_MAJOR)
AC_SUBST(VERSION_MINOR)
AC_SUBST(VERSION_MICRO)
AC_SUBST(VERSION_SUFFIX)
AC_SUBST(VERSION_SUFFIX_VERSION)
AC_SUBST(VERSION_FULL)

VERSION=$VERSION_FULL

AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE(herdstat, $VERSION_FULL)

test x$localstatedir = "xNONE" && localstatedir="$ac_default_localstatedir"
test x$sysconfdir = "xNONE" && sysconfdir="$ac_default_sysconfdir"
AC_DEFINE_UNQUOTED(LOCALSTATEDIR, "$localstatedir/herdstat", [Local state directory])
AC_DEFINE_UNQUOTED(SYSCONFDIR, "$sysconfdir", [System configuration directory])

dnl --enable-debug
AC_MSG_CHECKING([whether to enable debugging code/flags])
AC_ARG_ENABLE(debug,
    AC_HELP_STRING([--enable-debug],[Enable debugging code/flags]),
    [enable_debug=$enableval],[enable_debug=no])
AM_CONDITIONAL(DEBUG, test x$enable_debug != "xno")

if test x$enable_debug != "xno" ; then
    CXXFLAGS="-ggdb3 -Winline"
    AC_MSG_RESULT([$CXXFLAGS])
    AC_DEFINE(DEBUG, 1, [Enable debugging code])
else
    AC_MSG_RESULT(no)
fi

AC_LANG_CPLUSPLUS
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_INSTALL

dnl hack to disable damn f77 libtool checks (stolen from sandbox's configure.in)
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])
AC_PROG_LIBTOOL

LIBTOOL="${LIBTOOL} --silent"
AC_SUBST(LIBTOOL)

dnl SED is defined by the LT_AC_PROG_SED ran by AC_PROG_LIBTOOL
AC_SUBST(SED)

AC_MSG_CHECKING([gcc version])
gccver=`gcc -dumpversion | cut -f1 -d.`
AC_MSG_RESULT([$gccver])
if test x$gccver = "x4" ; then
    AC_DEFINE_UNQUOTED(HAVE_GCC4, 1, [GCC Version 4])
fi

dnl Required headers
AC_HEADER_STDC

dnl Optional headers
AC_CHECK_HEADERS(getopt.h)

dnl Required functions

dnl Optional functions
AC_CHECK_FUNCS(getopt_long)

dnl Optional libs

dnl Check for termcap lib
AC_MSG_CHECKING([whether to use ncurses to get screen width for output formatting])
AC_ARG_WITH(ncurses,
    AC_HELP_STRING([--with-ncurses],[Use ncurses to determine terminal width used for output formatting [on]]),
    [WITH_NCURSES=$withval],[WITH_NCURSES=yes])
AC_MSG_RESULT([$WITH_NCURSES])

if test "$WITH_NCURSES" = "yes" ; then
    AC_CHECK_LIB(ncurses, tgetent, [TERMCAP_LIBS="-lncurses"])
    AC_CHECK_HEADERS([curses.h term.h])
    AC_DEFINE(HAVE_NCURSES, 1, [Use ncurses to determine terminal width])
fi
    
AC_SUBST(TERMCAP_LIBS)

dnl Required libs
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES(libherdstat, libherdstat-0.1)

AM_CONFIG_HEADER(config.h)
AC_OUTPUT(Makefile
	  doc/Makefile
	  src/Makefile
	  src/io/Makefile
	  src/action/Makefile
	  tests/Makefile)
