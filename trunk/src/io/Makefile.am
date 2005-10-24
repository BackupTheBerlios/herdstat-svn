# $Id$

include $(top_builddir)/Makefile.am.common

noinst_LTLIBRARIES = libio.la
libio_la_SOURCES = \
	handler.hh handler.cc \
	stream.hh stream.cc \
	readline.hh readline.cc \
	batch.hh batch.cc

INCLUDES = -I$(top_builddir)/src $(libherdstat_CFLAGS)
MAINTAINERCLEANFILES = Makefile.in *~ *.loT
