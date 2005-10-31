# $Id$

SUBDIRS = action gui

INCLUDES = -I$(top_builddir)/src $(libherdstat_CFLAGS)
MAINTAINERCLEANFILES = Makefile.in *~ *.loT

noinst_LTLIBRARIES = libio.la
libio_la_SOURCES = \
	handler.hh handler.cc \
	stream.hh stream.cc \
	batch.hh batch.cc \
	gui.hh gui.cc

libio_la_LIBADD = \
	action/libio_action.la \
	gui/libio_gui.la

if BUILD_RL_FRONTEND
libio_la_SOURCES += readline.hh readline.cc
endif

srchtml: $(foreach f, $(libio_la_SOURCES), $(f).html)
%.cc.html: %.cc
	@echo Generating $@
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? -o ../../html/src/io/$@
%.hh.html: %.hh
	@echo Generating $@
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? -o ../../html/src/io/$@
