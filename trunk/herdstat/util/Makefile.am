# $Id$

include $(top_builddir)/Makefile.am.common

cc_sources = string.cc \
	     misc.cc \
	     regex.cc \
	     file.cc \
	     vars.cc \
	     glob.cc \
	     timer.cc \
	     progress.cc

hh_sources = string.hh \
	     misc.hh \
	     regex.hh \
	     file.hh \
	     vars.hh \
	     glob.hh \
	     timer.hh \
	     progress.hh \
	     functional.hh \
	     algorithm.hh

noinst_LTLIBRARIES = libutil.la
libutil_la_SOURCES = $(cc_sources) $(hh_sources)
libutil_la_LIBADD  = @TERMCAP_LIBS@

# library_includedir=$(includedir)/$(PACKAGE)-$(VERSION_MAJOR).$(VERSION_MINOR)/$(PACKAGE)/util
# library_include_HEADERS = $(hh_sources)

MAINTAINERCLEANFILES = Makefile.in *~ .loT

srchtml: $(foreach f, $(libutil_la_SOURCES), $(f).html)

%.cc.html: %.cc
	@echo Generating $@
	@test -d $(top_srcdir)/html/herdstat/util || mkdir $(top_srcdir)/html/herdstat/util
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? -o $(top_srcdir)/html/herdstat/util/$@

%.hh.html: %.hh
	@echo Generating $@
	@test -d $(top_srcdir)/html/herdstat/util || mkdir $(top_srcdir)/html/herdstat/util
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? -o $(top_srcdir)/html/herdstat/util/$@

