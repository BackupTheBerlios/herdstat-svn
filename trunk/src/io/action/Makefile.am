# $Id$

include $(top_builddir)/Makefile.am.common

INCLUDES = -I$(top_builddir)/src $(libherdstat_CFLAGS)
MAINTAINERCLEANFILES = Makefile.in *~ *.loT

noinst_LTLIBRARIES = libio_action.la
libio_action_la_SOURCES = \
	handler.hh handler.cc \
	help.hh help.cc

if BUILD_RL_FRONTEND
libio_action_la_SOURCES += set.hh set.cc print.hh print.cc
else
EXTRA_DIST = set.hh set.cc print.hh print.cc
endif

srchtml: $(foreach f, $(libio_action_la_SOURCES), $(f).html)
%.cc.html: %.cc
	@echo Generating $@
	@source-highlight -c ../../../src.css -s cpp -f html -t4 -n -i $? -o ../../../html/src/io/$@
%.hh.html: %.hh
	@echo Generating $@
	@source-highlight -c ../../../src.css -s cpp -f html -t4 -n -i $? -o ../../../html/src/io/$@
