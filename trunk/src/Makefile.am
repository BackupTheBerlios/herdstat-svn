# $Id$

bin_PROGRAMS = herdstat

herdstat_SOURCES = common.hh common.cc \
		   exceptions.hh \
		   options.hh options.cc \
		   devs.hh \
		   herds.hh herds.cc \
		   xmlparser.hh \
		   herds_xml_handler.hh herds_xml_handler.cc \
		   metadata_xml_handler.hh metadata_xml_handler.cc \
		   formatter.hh formatter.cc \
		   categories.hh \
		   cache.hh \
		   metadatas.hh metadatas.cc \
		   action_handler.hh \
		   action_herd_handler.hh action_herd_handler.cc \
		   action_dev_handler.hh action_dev_handler.cc \
		   action_pkg_handler.hh action_pkg_handler.cc \
		   action_meta_handler.hh action_meta_handler.cc \
		   action_stats_handler.hh action_stats_handler.cc \
		   herdstat.cc

herdstat_LDADD = ../lib/libutil.la

INCLUDES = -I../lib
LIBS = @LIBS@ @XMLWRAPP_LIBS@
AM_CXXFLAGS = @XMLWRAPP_CFLAGS@ -pedantic -ansi -Wall

MAINTAINERCLEANFILES = Makefile.in *~

srchtml:
	for x in $(herdstat_SOURCES) ; do \
		gvim +TOhtml "+w! $${x}.html" +q! +q! +q $${x} ; \
	done
