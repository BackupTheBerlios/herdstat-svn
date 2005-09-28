# $Id$

include $(top_builddir)/Makefile.am.common

cc_sources = wgetfetcher.cc \
	     curlfetcher.cc \
	     fetcher.cc

hh_sources = options.hh \
	     fetcherimp.hh \
	     wgetfetcher.hh \
	     curlfetcher.hh \
	     fetcher.hh

noinst_LTLIBRARIES = libfetcher.la
libfetcher_la_SOURCES = $(hh_sources) $(cc_sources)
libfetcher_la_LIBADD  = @CURL_LIBS@

MAINTAINERCLEANFILES = Makefile.in *~ *.loT

srchtml: $(foreach f, $(libfetcher_la_SOURCES), $(f).html)

%.cc.html: %.cc
	@echo Generating $@
	@test -d $(top_srcdir)/html/herdstat/fetcher || \
		mkdir $(top_srcdir)/html/herdstat/fetcher
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? -o \
		$(top_srcdir)/html/herdstat/fetcher/$@

%.hh.html: %.hh
	@echo Generating $@
	@test -d $(top_srcdir)/html/herdstat/fetcher || \
		mkdir $(top_srcdir)/html/herdstat/fetcher
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? -o \
		$(top_srcdir)/html/herdstat/fetcher/$@

