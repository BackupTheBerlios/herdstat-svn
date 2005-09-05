# Process this file with autoconf to produce a configure script.
# $Id: configure.ac 246 2005-04-26 12:03:53Z ka0ttic $

AC_PREREQ(2.59)
AC_INIT(src/herdstat.cc)

# release versioning
VERSION_MAJOR=1
VERSION_MINOR=1
VERSION_MICRO=2
VERSION_SUFFIX=alpha
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

# shared lib versioning
LIBRARY_VERSION=1:0:0
#               | | |
#        +------+ | +---+
#        |        |     |
#        current:revision:age
#        |        |     |
#        |        |     +- increment if interfaces have been added
#        |        |        set to zero if interfaces have been removed
#                          or changed
#        |        +- increment if source code has changed
#        |           set to zero if current is incremented
#        +- increment if interfaces have been added, removed or changed
AC_SUBST(LIBRARY_VERSION)

VERSION=$VERSION_FULL

AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE(herdstat, $VERSION_FULL)

test x$localstatedir = "xNONE" && localstatedir="$ac_default_localstatedir"
test x$sysconfdir = "xNONE" && sysconfdir="$ac_default_sysconfdir"
AC_DEFINE_UNQUOTED(LOCALSTATEDIR, "$localstatedir", [Local state directory])
AC_DEFINE_UNQUOTED(SYSCONFDIR, "$sysconfdir", [System configuration directory])

AC_CHECK_PROGS(_WGET, wget)
if test x$_WGET = "x"; then
    AC_MSG_ERROR([wget is required])
fi
AC_DEFINE_UNQUOTED(WGET, "$_WGET", [wget location])

dnl build API docs?
AC_MSG_CHECKING([whether to build API docs for libherdstat])
AC_ARG_ENABLE(doc,
    AC_HELP_STRING([--enable-doc],[Enable API docs for libherdstat [off]]),
    [enable_doc=$enableval],[enable_doc=no])
AM_CONDITIONAL(GENERATE_API_DOCS, test x$enable_doc != "xno")
AC_MSG_RESULT([$enable_doc])

if test x$enable_doc != "xno" ; then
    AC_CHECK_PROGS(DOXYGEN, doxygen)
    if test x$DOXYGEN = "x" ; then
	AC_MSG_ERROR([doxygen is required to build API docs])
    fi
fi

dnl --enable-debug
AC_MSG_CHECKING([whether to enable debugging code/flags])
AC_ARG_ENABLE(debug,
    AC_HELP_STRING([--enable-debug],[Enable debugging code/flags]),
    [enable_debug=$enableval],[enable_debug=no])
AM_CONDITIONAL(DEBUG, test x$enable_debug != "xno")

if test x$enable_debug != "xno" ; then
    CXXFLAGS="-Weffc++ -ggdb3 -O0"
    AC_MSG_RESULT([$CXXFLAGS])
    AC_DEFINE(DEBUG, 1, [Enable debugging code])
else
    AC_MSG_RESULT(no)
fi

AC_LANG_CPLUSPLUS
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_INSTALL

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
AC_CHECK_FUNCS(vasprintf,,
    [AC_MSG_ERROR(vasprintf is required)])

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

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES(xmlwrapp, xmlwrapp >= 0.5.0)

AM_CONFIG_HEADER(config.h)
AC_OUTPUT(Makefile
	  doc/Makefile
	  herdstat/Makefile
	  herdstat/util/Makefile
	  herdstat/xml/Makefile
	  herdstat/portage/Makefile
	  src/Makefile
	  tests/Makefile
	  tests/src/Makefile)
