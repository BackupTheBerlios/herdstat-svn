# $Id$

include $(top_builddir)/Makefile.am.common

SUBDIRS = io action

bin_PROGRAMS = herdstat

herdstat_SOURCES = common.hh common.cc \
		   exceptions.hh \
		   options.hh options.cc \
		   rc.hh rc.cc \
		   formatter.hh formatter.cc \
		   pkgquery.hh pkgquery.cc \
		   pkgcache.hh pkgcache.cc \
		   metacache.hh metacache.cc \
		   querycache.hh querycache.cc \
		   overlaydisplay.hh overlaydisplay.cc \
		   query.hh query.cc \
		   query_results.hh \
		   fields.hh \
		   getcols.cc \
		   main.cc

herdstat_LDADD = $(libherdstat_LIBS) @TERMCAP_LIBS@

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
querycachectl_LDADD = $(libherdstat_LIBS)
endif

INCLUDES = -I.. $(libherdstat_CFLAGS)
MAINTAINERCLEANFILES = Makefile.in *~

srchtml: $(foreach f, $(herdstat_SOURCES), $(f).html)
%.cc.html: %.cc
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/src/$@
%.hh.html: %.hh
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/src/$@
