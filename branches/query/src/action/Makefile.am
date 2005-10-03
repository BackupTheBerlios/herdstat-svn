# $Id$

include $(top_builddir)/Makefile.am.common

cc_sources = dev.cc
hh_sources = dev.hh

noinst_LTLIBRARIES = libaction.la
libaction_la_SOURCES = $(hh_sources) $(cc_sources)

INCLUDES += -I..
MAINTAINERCLEANFILES = Makefile.in *~ *.loT
