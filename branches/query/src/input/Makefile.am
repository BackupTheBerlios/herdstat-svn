# $Id$

include $(top_builddir)/Makefile.am.common

cc_sources = readline.cc
hh_sources = readline.hh

noinst_LTLIBRARIES = libinput.la
libinput_la_SOURCES = $(hh_sources) $(cc_sources)
libinput_la_LIBADD = @READLINE_LIBS@

INCLUDES += -I..
MAINTAINERCLEANFILES = Makefile.in *~ *.loT
