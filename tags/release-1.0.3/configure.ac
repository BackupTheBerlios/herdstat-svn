# Process this file with autoconf to produce a configure script.
# $Id$

AC_INIT([herdstat], [1.0.3], [ka0ttic@gentoo.org])
AC_CONFIG_SRCDIR([src/herdstat.cc])
AC_CONFIG_AUX_DIR([config])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.8 foreign dist-bzip2 no-dist-gzip])

test x$localstatedir = "xNONE" && localstatedir="$ac_default_localstatedir"
AC_DEFINE_UNQUOTED(LOCALSTATEDIR, "$localstatedir/herdstat", [Local state directory])

AC_CHECK_PROGS(SED, sed)
if test x$SED = "x"; then
    AC_MSG_ERROR([sed is required])
fi

AC_CHECK_PROGS(_WGET, wget)
if test x$_WGET = "x"; then
    AC_MSG_ERROR([wget is required])
fi
AC_DEFINE_UNQUOTED(WGET, "$_WGET", [wget location])

AC_LANG_CPLUSPLUS
AC_PROG_CXX
AC_PROG_INSTALL

dnl Required headers
AC_HEADER_STDC
AC_HEADER_DIRENT(,
    [AC_MSG_ERROR([dirent.h is required])])
AC_CHECK_HEADERS(sys/time.h,,
    [AC_MSG_ERROR([sys/time.h is required])])

dnl Required funcs
AC_CHECK_FUNCS(asprintf,,
    [AC_CHECK_FUNCS(snprintf,,
    [AC_MSG_ERROR([either asprintf or snprintf is required])])])
AC_CHECK_FUNCS(vasprintf,,
    [AC_CHECK_FUNCS(vsnprintf,,
    [AC_CHECK_FUNCS(vsprintf,,
    [AC_MSG_ERROR([either vasprintf, vsnprintf, or vsprintf is required])])])])

dnl Optional headers
AC_CHECK_HEADERS(getopt.h)

dnl Optional functions
AC_CHECK_FUNCS(getopt_long)

PKG_CHECK_MODULES(XMLWRAPP, xmlwrapp >= 0.5)

AC_ARG_ENABLE(debug, [  --enable-debug      enable debugging flags [off]],
    [case "${enableval}" in
        yes)    debug=true
                CXXFLAGS="-ggdb3"
                AC_DEFINE_UNQUOTED(DEBUG, 1, [Enable debugging code])
                ;;
        no)     debug=false ;;
        *)      AC_MSG_ERROR(--enable-debug accepts yes or no only) ;;
    esac], [debug=false])

AM_CONFIG_HEADER(config.h)
AC_OUTPUT(Makefile
	  src/Makefile
	  doc/Makefile)
