# Process this file with autoconf to produce a configure script.
# $Id: configure.ac 246 2005-04-26 12:03:53Z ka0ttic $

AC_PREREQ(2.59)
AC_INIT([herdstat], [1.1.1_p2], [ka0ttic@gentoo.org])
AC_CONFIG_SRCDIR([src/herdstat.cc])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([1.8 dist-bzip2 no-dist-gzip])

test x$localstatedir = "xNONE" && localstatedir="$ac_default_localstatedir"
test x$sysconfdir = "xNONE" && sysconfdir="$ac_default_sysconfdir"
AC_DEFINE_UNQUOTED(LOCALSTATEDIR, "$localstatedir/herdstat", [Local state directory])
AC_DEFINE_UNQUOTED(SYSCONFDIR, "$sysconfdir", [System configuration directory])

AC_CHECK_PROGS(_WGET, wget)
if test x$_WGET = "x"; then
    AC_MSG_ERROR([wget is required])
fi
AC_DEFINE_UNQUOTED(WGET, "$_WGET", [wget location])

dnl build API docs?
AC_MSG_CHECKING([whether to build API docs for internal libutil])
AC_ARG_ENABLE([api-docs],
    [ --enable-api-docs  enable API docs for internal libutil.la [off]],
    enable_api_docs=$enableval, enable_api_docs=no)
AM_CONDITIONAL(GENERATE_API_DOCS, test x$enable_api_docs = xyes)
AC_MSG_RESULT($enable_api_docs)

if test x$enable_api_docs = "xyes" ; then
    AC_CHECK_PROGS(DOXYGEN, doxygen)
    if test x$DOXYGEN = "x" ; then
	AC_MSG_ERROR([doxygen is required to build API docs])
    fi
fi

AC_LANG_CPLUSPLUS
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_INSTALL

AC_MSG_CHECKING([devrel wang size])
AC_MSG_RESULT(huge)

AC_DISABLE_SHARED
dnl hack to disable damn f77 libtool checks (stolen from sandbox's configure.in)
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])
AC_PROG_LIBTOOL

LIBTOOL="${LIBTOOL} --silent"
AC_SUBST(LIBTOOL)

dnl SED is defined by the LT_AC_PROG_SED ran by AC_PROG_LIBTOOL
AC_SUBST(SED)

AC_MSG_CHECKING([gcc version])
gccver=`${CC} -dumpversion | cut -f1 -d.`
AC_MSG_RESULT([$gccver])
if test x$gccver = "x4" ; then
    AC_DEFINE_UNQUOTED(HAVE_GCC4, 1, [GCC Version 4])
fi

dnl Required headers
AC_HEADER_STDC
AC_HEADER_TIME(,
    [AC_MSG_ERROR([sys/time.h is required])])
AC_HEADER_DIRENT(,
    [AC_MSG_ERROR([dirent.h is required])])
AC_CHECK_HEADERS(regex.h,,
    [AC_MSG_ERROR([regex.h is required])])
AC_CHECK_HEADERS(glob.h,,
    [AC_MSG_ERROR([glob.h is required])])
AC_CHECK_HEADERS(fnmatch.h,,
    [AC_MSG_ERROR([fnmatch.h is required])])

dnl Optional headers
AC_CHECK_HEADERS(getopt.h)

dnl Required functions
AC_FUNC_CLOSEDIR_VOID
AC_CHECK_FUNCS(gettimeofday,,
    [AC_MSG_ERROR([gettimeofday is required])])
AC_CHECK_FUNCS(regcomp,,
    [AC_MSG_ERROR([regcomp is required])])
AC_CHECK_FUNCS(regexec,,
    [AC_MSG_ERROR([regexec is required])])
AC_CHECK_FUNCS(regfree,,
    [AC_MSG_ERROR([regfree is required])])
AC_CHECK_FUNCS(regerror,,
    [AC_MSG_ERROR([regerror is required])])
AC_CHECK_FUNCS(glob,,
    [AC_MSG_ERROR([glob is required])])
AC_CHECK_FUNCS(fnmatch,,
    [AC_MSG_ERROR([fnmatch is required])])

dnl Optional functions
AC_CHECK_FUNCS(getopt_long)

dnl Required libs

dnl Check for termcap lib
for termlib in ncurses curses termcap terminfo termlib ; do
    AC_CHECK_LIB(${termlib}, tgetent,
	[TERMCAP_LIBS="$TERMCAP_LIBS -l$termlib"])
    case "$TERMCAP_LIBS" in
	*-l${termlib}*) break ;;
    esac
done

if test -z "$TERMCAP_LIBS" ; then
    AC_MSG_ERROR([a termcap library is required])
fi

AC_SUBST(TERMCAP_LIBS)
AC_DEFINE(USE_TERMCAP, 1, [Use termcap lib to determine terminal width])

dnl --enable-debug
AC_ARG_ENABLE(debug, [  --enable-debug      enable debugging flags [off]],
    [case "${enableval}" in
        yes)    debug=true
                CXXFLAGS="-ggdb3"
                AC_DEFINE_UNQUOTED(DEBUG, 1, [Enable debugging code])
                ;;
        no)     debug=false ;;
        *)      AC_MSG_ERROR(--enable-debug accepts yes or no only) ;;
    esac], [debug=false])
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)

dnl --enable-unicode
AC_ARG_ENABLE(unicode, [ --enable-unicode   enable Unicode support [on]],
    enable_unicode=$enableval, enable_unicode=yes)

PKG_PROG_PKG_CONFIG
if test x$enable_unicode = "xyes"; then
    AC_DEFINE_UNQUOTED(UNICODE, 1, [Enable Unicode support])
    PKG_CHECK_MODULES(GLIBMM, glibmm-2.4 >= 2.4)
    PKG_CHECK_MODULES(LIBXMLPP, libxml++-2.6 >= 2.6)
else
    AC_CHECK_FUNCS(vasprintf,,
	[AC_MSG_ERROR(vasprintf is required)])
    PKG_CHECK_MODULES(XMLWRAPP, xmlwrapp >= 0.5.0)
fi

AM_CONFIG_HEADER(config.h)
AC_OUTPUT(Makefile
	  doc/Makefile
	  lib/Makefile
	  src/Makefile
	  tests/Makefile
	  tests/foo/Makefile)
