# $Id$

include $(top_builddir)/Makefile.am.common

noinst_LTLIBRARIES = libaction.la
libaction_la_SOURCES = handler.hh handler.cc \
		       herd.hh herd.cc

INCLUDES = -I.. $(libherdstat_CFLAGS)
MAINTAINERCLEANFILES = Makefile.in *~ *.loT
