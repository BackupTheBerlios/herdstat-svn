# $Id$

include $(top_builddir)/Makefile.am.common

cc_sources = stream.cc
hh_sources = stream.hh

noinst_LTLIBRARIES = liboutput.la
liboutput_la_SOURCES = $(hh_sources) $(cc_sources)

INCLUDES += -I..
MAINTAINERCLEANFILES = Makefile.in *~ *.loT
