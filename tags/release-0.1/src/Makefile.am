# $Id: Makefile.am 73 2005-03-04 10:43:57Z ka0ttic $

bin_PROGRAMS = herdstat

herdstat_SOURCES = exceptions.hh \
		   options.hh options.cc \
		   util.hh util.cc \
		   xmlparser.hh \
		   types.hh \
		   herds.hh herds.cc \
		   metadata.hh metadata.cc \
		   formatter.hh formatter.cc \
		   action_handler.hh \
		   action_herd_handler.hh action_herd_handler.cc \
		   action_dev_handler.hh action_dev_handler.cc \
		   action_pkg_handler.hh action_pkg_handler.cc \
		   herdstat.cc

LIBS = @LIBS@ @XMLWRAPP_LIBS@
AM_CXXFLAGS = @XMLWRAPP_CFLAGS@ -pedantic -ansi -Wall

MAINTAINERCLEANFILES = Makefile.in *~
