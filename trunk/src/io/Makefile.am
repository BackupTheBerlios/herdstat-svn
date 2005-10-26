# $Id$

SUBDIRS = gui

INCLUDES = -I$(top_builddir)/src $(libherdstat_CFLAGS)
MAINTAINERCLEANFILES = Makefile.in *~ *.loT

noinst_LTLIBRARIES = libio.la
libio_la_SOURCES = \
	handler.hh handler.cc \
	stream.hh stream.cc \
	batch.hh batch.cc
libio_la_LIBADD = gui/libgui.la

if BUILD_RL_FRONTEND
libio_la_SOURCES += readline.hh readline.cc
endif
