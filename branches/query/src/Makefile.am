# $Id$

include $(top_builddir)/Makefile.am.common

SUBDIRS = io

bin_PROGRAMS = herdstat

herdstat_SOURCES = common.hh common.cc \
		   exceptions.hh \
		   options.hh options.cc \
		   rc.hh rc.cc \
		   pkgquery.hh pkgquery.cc \
		   pkgcache.hh pkgcache.cc \
		   metacache.hh metacache.cc \
		   querycache.hh querycache.cc \
		   formatter.hh formatter.cc \
		   overlaydisplay.hh \
		   fields.hh \
		   action_handler.hh action_handler.cc \
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

herdstat_LDADD = $(top_builddir)/herdstat/libherdstat.la

if DEBUG
bin_PROGRAMS += querycachectl
querycachectl_SOURCES = common.hh common.cc \
			exceptions.hh \
			options.hh options.cc \
			rc.hh rc.cc \
			formatter.hh formatter.cc \
			pkgquery.hh pkgquery.cc \
			querycache.hh querycache.cc \
			querycachectl.cc
querycachectl_LDADD = $(top_builddir)/herdstat/libherdstat.la
endif

MAINTAINERCLEANFILES = Makefile.in *~

srchtml: $(foreach f, $(herdstat_SOURCES), $(f).html)
%.cc.html: %.cc
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/src/$@
%.hh.html: %.hh
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/src/$@
