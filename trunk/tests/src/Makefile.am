include $(top_builddir)/Makefile.am.common

noinst_PROGRAMS = string-test \
		  glob-test

string_test_SOURCES = string-test.cc
string_test_LDADD = $(top_builddir)/herdstat/libherdstat.la

glob_test_SOURCES = glob-test.cc
glob_test_LDADD = $(top_builddir)/herdstat/libherdstat.la

MAINTAINERCLEANFILES = Makefile.in *~ .loT
