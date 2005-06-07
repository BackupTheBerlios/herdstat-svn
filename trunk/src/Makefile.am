# $Id: Makefile.am 243 2005-04-22 13:30:17Z ka0ttic $

bin_PROGRAMS = herdstat

herdstat_SOURCES = common.hh common.cc \
		   exceptions.hh \
		   options.hh options.cc \
		   rc.hh rc.cc \
		   devs.hh \
		   fetcher.hh fetcher.cc \
		   herds.hh herds.cc \
		   pkgquery.hh pkgquery.cc \
		   metadata.hh metadata.cc \
		   pkgcache.hh pkgcache.cc \
		   metacache.hh metacache.cc \
		   querycache.hh querycache.cc \
		   xmlparser.hh \
		   querycache_xml_handler.hh querycache_xml_handler.cc \
		   herds_xml_handler.hh herds_xml_handler.cc \
		   metadata_xml_handler.hh metadata_xml_handler.cc \
		   parsable.hh \
		   xml.hh \
		   devaway.hh devaway.cc \
		   herds_xml.hh herds_xml.cc \
		   metadata_xml.hh \
		   formatter.hh formatter.cc \
		   overlaydisplay.hh \
		   action_handler.hh \
		   action_away_handler.hh action_away_handler.cc \
		   action_dev_handler.hh action_dev_handler.cc \
		   action_herd_handler.hh action_herd_handler.cc \
		   action_fetch_handler.hh action_fetch_handler.cc \
		   action_find_handler.hh action_find_handler.cc \
		   action_meta_handler.hh action_meta_handler.cc \
		   action_pkg_handler.hh action_pkg_handler.cc \
		   action_stats_handler.hh action_stats_handler.cc \
		   action_which_handler.hh action_which_handler.cc \
		   action_versions_handler.hh action_versions_handler.cc \
		   herdstat.cc

herdstat_LDADD = ../lib/libutil.la

if DEBUG
bin_PROGRAMS += querycachectl
querycachectl_SOURCES = common.hh common.cc \
			exceptions.hh \
			options.hh options.cc \
			rc.hh rc.cc \
			formatter.hh formatter.cc \
			pkgquery.hh pkgquery.cc \
			querycache.hh querycache.cc \
			xmlparser.hh \
			querycache_xml_handler.hh querycache_xml_handler.cc \
			parsable.hh \
			xml.hh \
			querycachectl.cc
querycachectl_LDADD = ../lib/libutil.la
endif

INCLUDES = -I../lib
LIBS = @LIBS@ @LIBXMLPP_LIBS@ @XMLWRAPP_LIBS@
AM_CXXFLAGS = -pedantic -ansi -Wall @LIBXMLPP_CFLAGS@ @XMLWRAPP_CFLAGS@

MAINTAINERCLEANFILES = Makefile.in *~

srchtml: $(foreach f, $(herdstat_SOURCES), $(f).html)
%.cc.html: %.cc
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/src/$@
%.hh.html: %.hh
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/src/$@
