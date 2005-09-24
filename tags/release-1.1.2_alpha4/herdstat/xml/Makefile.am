# $Id$

include $(top_builddir)/Makefile.am.common

cc_sources = init.cc \
	     saxparser.cc
hh_sources = exceptions.hh \
	     init.hh \
	     saxparser.hh \
	     document.hh

noinst_LTLIBRARIES = libxml.la
libxml_la_SOURCES = $(hh_sources) $(cc_sources)
libxml_la_LIBADD  = @xmlwrapp_LIBS@

# library_includedir=$(includedir)/$(PACKAGE)-$(VERSION_MAJOR).$(VERSION_MINOR)/$(PACKAGE)/xml
# library_include_HEADERS = $(hh_sources)

MAINTAINERCLEANFILES = Makefile.in *~ *.loT

srchtml: $(foreach f, $(libxml_la_SOURCES), $(f).html)

%.cc.html: %.cc
	@echo Generating $@
	@test -d $(top_srcdir)/html/herdstat/xml || mkdir $(top_srcdir)/html/herdstat/xml
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? -o $(top_srcdir)/html/herdstat/xml/$@

%.hh.html: %.hh
	@echo Generating $@
	@test -d $(top_srcdir)/html/herdstat/xml || mkdir $(top_srcdir)/html/herdstat/xml
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? -o $(top_srcdir)/html/herdstat/xml/$@
