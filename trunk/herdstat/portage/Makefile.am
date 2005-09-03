# $Id$

include $(top_builddir)/Makefile.am.common

cc_sources = misc.cc \
	     config.cc \
	     version.cc \
	     find.cc
hh_sources = exceptions.hh \
	     misc.hh \
	     config.hh \
             version.hh \
	     find.hh

noinst_LTLIBRARIES = libportage.la
libportage_la_SOURCES = $(cc_sources) $(hh_sources)

# library_includedir=$(includedir)/$(PACKAGE)-$(VERSION_MAJOR).$(VERSION_MINOR)/$(PACKAGE)/portage
# library_include_HEADERS = $(hh_sources)

srchtml: $(foreach f, $(libportage_la_SOURCES), $(f).html)
%.cc.html: %.cc
	@echo Generating $@
	@test -d $(top_srcdir)/html/herdstat/portage || \
		mkdir $(top_srcdir)/html/herdstat/portage
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? \
		-o $(top_srcdir)/html/herdstat/portage/$@
%.hh.html: %.hh
	@echo Generating $@
	@test -d $(top_srcdir)/html/herdstat/portage || \
		mkdir $(top_srcdir)/html/herdstat/portage
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? \
		-o $(top_srcdir)/html/herdstat/portage/$@
