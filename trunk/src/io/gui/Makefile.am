# $Id$

AM_CXXFLAGS = -pedantic -ansi -Wall -Woverloaded-virtual \
 -Wredundant-decls $(QT_CXXFLAGS)
INCLUDES = $(QT_INCLUDES) $(gtkmm_CFLAGS) \
 -I$(top_builddir)/src $(libherdstat_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~ *.loT

noinst_LTLIBRARIES = libio_gui.la
libio_gui_la_SOURCES = \
	widgets.hh widgets.cc \
	widget_factory.hh

libio_gui_la_LIBADD = $(QT_LIBS) $(gtkmm_LIBS)
libio_gui_la_LDFLAGS = $(QT_LDFLAGS) $(QT_RPATH)

if BUILD_QT_FRONTEND
libio_gui_la_SOURCES += qt_widget_factory.hh qt_widget_factory.cc
AM_CXXFLAGS += -Wno-long-long
else
AM_CXXFLAGS += -Wold-style-cast
endif

if BUILD_GTK_FRONTEND
libio_gui_la_SOURCES += gtk_widget_factory.hh gtk_widget_factory.cc
endif

srchtml: $(foreach f, $(libio_gui_la_SOURCES), $(f).html)
%.cc.html: %.cc
	@echo Generating $@
	@source-highlight -c ../../../src.css -s cpp -f html -t4 -n -i $? -o ../../../html/src/io/gui/$@
%.hh.html: %.hh
	@echo Generating $@
	@source-highlight -c ../../../src.css -s cpp -f html -t4 -n -i $? -o ../../../html/src/io/gui/$@
