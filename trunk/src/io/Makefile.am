# $Id$

AM_CXXFLAGS = -pedantic -ansi -Wall -Woverloaded-virtual -Wredundant-decls
INCLUDES = -I$(top_builddir)/src $(libherdstat_CFLAGS)
MAINTAINERCLEANFILES = Makefile.in *~ *.loT

noinst_LTLIBRARIES = libio.la
libio_la_SOURCES = \
	handler.hh handler.cc \
	stream.hh stream.cc \
	batch.hh batch.cc

if BUILD_RL_FRONTEND
libio_la_SOURCES += readline.hh readline.cc
endif

if BUILD_QT_FRONTEND
libio_la_SOURCES += qt.hh qt.cc
INCLUDES += $(QT_INCLUDES)
libio_la_LIBADD = $(QT_LIBS)
libio_la_LDFLAGS = $(QT_LDFLAGS) $(QT_RPATH)
AM_CXXFLAGS += -Wno-long-long $(QT_CXXFLAGS)
else
AM_CXXFLAGS += -Wold-style-cast
endif

if BUILD_GTK_FRONTEND
libio_la_SOURCES += gtk.hh gtk.cc
INCLUDES += $(gtkmm_CFLAGS)
endif
