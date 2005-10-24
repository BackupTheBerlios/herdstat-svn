# $Id$

include $(top_builddir)/Makefile.am.common

SUBDIRS = action io

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
		   fields.hh \
		   query.hh query.cc \
		   query_results.hh \
		   getcols.cc \
		   herdstat.cc

herdstat_LDADD = \
		action/libaction.la \
		io/libio.la \
		$(libherdstat_LIBS) \
		@TERMCAP_LIBS@

# if DEBUG
# bin_PROGRAMS += querycachectl
# querycachectl_SOURCES = common.hh common.cc \
#                         exceptions.hh \
#                         options.hh options.cc \
#                         rc.hh rc.cc \
#                         formatter.hh formatter.cc \
#                         pkgquery.hh pkgquery.cc \
#                         querycache.hh querycache.cc \
#                         querycachectl.cc
# querycachectl_LDADD = $(libherdstat_LIBS)
# endif

INCLUDES = $(libherdstat_CFLAGS)
MAINTAINERCLEANFILES = Makefile.in *~

srchtml: $(foreach f, $(herdstat_SOURCES), $(f).html)
%.cc.html: %.cc
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/src/$@
%.hh.html: %.hh
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/src/$@
