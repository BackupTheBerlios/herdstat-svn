# $Id: Makefile.am 104 2005-03-06 19:47:53Z ka0ttic $

bin_PROGRAMS = herdstat

herdstat_SOURCES = exceptions.hh \
		   options.hh options.cc \
		   util.hh util.cc \
		   devs.hh \
		   herds.hh herds.cc \
		   xmlparser.hh \
		   herds_xml_handler.hh herds_xml_handler.cc \
		   metadata_xml_handler.hh metadata_xml_handler.cc \
		   formatter.hh formatter.cc \
		   action_handler.hh \
		   action_herd_handler.hh action_herd_handler.cc \
		   action_dev_handler.hh action_dev_handler.cc \
		   action_pkg_handler.hh action_pkg_handler.cc \
		   herdstat.cc

LIBS = @LIBS@ @XMLWRAPP_LIBS@
AM_CXXFLAGS = @XMLWRAPP_CFLAGS@ -pedantic -ansi -Wall

MAINTAINERCLEANFILES = Makefile.in *~
