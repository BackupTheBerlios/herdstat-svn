# $Id$

AM_CXXFLAGS = -pedantic -ansi -Wall -Woverloaded-virtual \
	-Wredundant-decls $(QT_CXXFLAGS)
INCLUDES = -I$(top_builddir)/src $(libherdstat_CFLAGS) $(QT_INCLUDES) $(gtkmm_CFLAGS)
MAINTAINERCLEANFILES = Makefile.in *~ *.loT

noinst_LTLIBRARIES = libio.la
libio_la_SOURCES = \
	handler.hh handler.cc \
	stream.hh stream.cc \
	batch.hh batch.cc

libio_la_LIBADD = $(QT_LIBS) $(gtkmm_LIBS)
libio_la_LDFLAGS = $(QT_LDFLAGS) $(QT_RPATH)

if BUILD_RL_FRONTEND
libio_la_SOURCES += readline.hh readline.cc
endif

if BUILD_QT_FRONTEND
libio_la_SOURCES += qt.hh qt.cc
AM_CXXFLAGS += -Wno-long-long
else
AM_CXXFLAGS += -Wold-style-cast
endif

if BUILD_GTK_FRONTEND
libio_la_SOURCES += gtk.hh gtk.cc
endif
