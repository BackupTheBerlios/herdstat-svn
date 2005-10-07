# $Id$

include $(top_builddir)/Makefile.am.common

cc_sources = handler.cc \
	herd.cc
hh_sources = handler.hh \
	herd.hh

noinst_LTLIBRARIES = libaction.la
libaction_la_SOURCES = $(hh_sources) $(cc_sources)

INCLUDES += -I..
MAINTAINERCLEANFILES = Makefile.in *~ *.loT
