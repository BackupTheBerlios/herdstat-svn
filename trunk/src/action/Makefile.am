# $Id$

include $(top_builddir)/Makefile.am.common

noinst_LTLIBRARIES = libaction.la
libaction_la_SOURCES = \
	handler.hh handler.cc \
	away.hh away.cc \
	dev.hh dev.cc \
	find.hh find.cc \
	herd.hh herd.cc \
	keywords.hh keywords.cc \
	meta.hh meta.cc \
	pkg.hh pkg.cc \
	stats.hh stats.cc \
	versions.hh versions.cc \
	which.hh which.cc

INCLUDES = -I$(top_builddir)/src $(libherdstat_CFLAGS)
MAINTAINERCLEANFILES = Makefile.in *~ *.loT

srchtml: $(foreach f, $(libaction_la_SOURCES), $(f).html)
%.cc.html: %.cc
	@echo Generating $@
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? -o ../../html/src/action/$@
%.hh.html: %.hh
	@echo Generating $@
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? -o ../../html/src/action/$@
