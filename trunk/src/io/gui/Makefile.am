# $Id$

AM_CXXFLAGS = -pedantic -ansi -Wall -Woverloaded-virtual \
 -Wredundant-decls $(QT_CXXFLAGS)
INCLUDES = $(QT_INCLUDES) $(gtkmm_CFLAGS) \
 -I$(top_builddir)/src $(libherdstat_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~ *.loT

noinst_LTLIBRARIES = libgui.la
libgui_la_SOURCES = \
	widgets.hh \
	gui_factory.hh

libgui_la_LIBADD = $(QT_LIBS) $(gtkmm_LIBS)
libgui_la_LDFLAGS = $(QT_LDFLAGS) $(QT_RPATH)

if BUILD_QT_FRONTEND
libgui_la_SOURCES += qt_factory.hh qt_factory.cc
AM_CXXFLAGS += -Wno-long-long
else
AM_CXXFLAGS += -Wold-style-cast
endif

if BUILD_GTK_FRONTEND
libgui_la_SOURCES += gtk_factory.hh gtk_factory.cc
endif
