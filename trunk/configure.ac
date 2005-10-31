# Process this file with autoconf to produce a configure script.
# $Id: configure.ac 246 2005-04-26 12:03:53Z ka0ttic $

AC_PREREQ(2.59)
AC_INIT(src/herdstat.cc)

# release versioning
VERSION_MAJOR=1
VERSION_MINOR=1
VERSION_MICRO=90
VERSION_SUFFIX=
VERSION_SUFFIX_VERSION=
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

dnl --with-test-data
AC_ARG_WITH(test-data,
    AC_HELP_STRING([--with-test-data],[Test data directory]),
    [TEST_DATA=$withval], [TEST_DATA=foo])
if test x"$TEST_DATA" = "xfoo" ; then
    TEST_DATA="${PWD}/../../libherdstat/test-data"
fi
AC_SUBST(TEST_DATA)

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

if test x"$WITH_NCURSES" = "xyes" ; then
    AC_CHECK_LIB(ncurses, tgetent)
    AC_CHECK_HEADERS([curses.h term.h])
    AC_DEFINE(HAVE_NCURSES, 1, [Use ncurses to determine terminal width])
fi

dnl --with-readline
AC_MSG_CHECKING([whether to build the readline front-end])
AC_ARG_WITH(readline,
    AC_HELP_STRING([--with-readline],[Enable the readline front-end]),
    [WITH_READLINE=$withval],[WITH_READLINE=yes])
AC_MSG_RESULT([$WITH_READLINE])

if test x"$WITH_READLINE" = "xyes" ; then
    AC_CHECK_HEADERS(readline/readline.h,,
	[AC_MSG_ERROR([readline support was enabled but no readline/readline.h was found])])
    AC_CHECK_LIB(readline, readline)
    AC_DEFINE(READLINE_FRONTEND, 1, [Enable Readline front-end])
fi

PKG_PROG_PKG_CONFIG

dnl --with-qt
AC_MSG_CHECKING([whether to build the Qt front-end])
AC_ARG_WITH(qt,
    AC_HELP_STRING([--with-qt],[Enable the Qt front-end]),
    [WITH_QT=$withval],[WITH_QT=yes])
AC_MSG_RESULT([$WITH_QT])

if test x"$WITH_QT" = "xyes" ; then
#    QT_CHECK_COMPILERS
    QT_PATH

    if test x"$have_qt" = "xno" ; then
	AC_MSG_ERROR([[
***
*** Qt ($qt_minversion) $ac_qt_notfound not found.  Please check your
*** installation!  For more details about this problem, look at the
*** end of config.log.$missing_qt_mt
***]])

    fi

    if test x"$have_moc" = "xno" ; then
	AC_MSG_ERROR([[
***
*** No Qt meta object compiler (moc) found!
*** Please check whether you installed Qt correctly.
*** You need to have a running moc binary.
*** configure tried to run $ac_cv_path_moc and the test didn't
*** succeed. If configure shouldn't have tried this one, set
*** the environment variable MOC to the right one before running
*** configure.
***]])

    fi

    AC_DEFINE(QT_FRONTEND, 1, [Enable Qt front-end])
fi

AC_MSG_CHECKING([whether to build the GTK+ front-end])
AC_ARG_WITH(gtk,
    AC_HELP_STRING([--with-gtk],[Enable the GTK+ front-end]),
    [WITH_GTK=$withval],[WITH_GTK=yes])
AC_MSG_RESULT([$WITH_GTK])

if test x"$WITH_GTK" = "xyes" ; then
    PKG_CHECK_MODULES(gtkmm, gtkmm-2.4)
    AC_DEFINE(GTK_FRONTEND, 1, [Enable GTK+ front-end])
fi

AM_CONDITIONAL(BUILD_RL_FRONTEND, test x"$WITH_READLINE" = "xyes")
AM_CONDITIONAL(BUILD_QT_FRONTEND, test x"$WITH_QT" = "xyes")
AM_CONDITIONAL(BUILD_GTK_FRONTEND, test x"$WITH_GTK" = "xyes")

dnl Required libs
PKG_CHECK_MODULES(libherdstat, libherdstat-0.1)

AM_CONFIG_HEADER(config.h)
AC_OUTPUT(Makefile
	  doc/Makefile
	  src/Makefile
	  src/action/Makefile
	  src/io/Makefile
	  src/io/action/Makefile
	  src/io/gui/Makefile
	  tests/Makefile)
