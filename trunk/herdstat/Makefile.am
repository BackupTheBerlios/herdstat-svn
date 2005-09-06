# $Id$

include $(top_builddir)/Makefile.am.common

SUBDIRS = util xml portage

cc_sources = fetcher.cc
hh_sources = exceptions.hh \
	     noncopyable.hh \
	     cachable.hh \
	     fetcher.hh \
	     fetchable.hh

noinst_LTLIBRARIES = libherdstat.la
# libherdstat_la_LDFLAGS = -release $(VERSION_MAJOR).$(VERSION_MINOR) -version-info $(LIBRARY_VERSION)
libherdstat_la_LIBADD = util/libutil.la \
			xml/libxml.la \
			portage/libportage.la
libherdstat_la_SOURCES = $(hh_sources) $(cc_sources)

# library_includedir=$(includedir)/$(PACKAGE)-$(VERSION_MAJOR).$(VERSION_MINOR)/$(PACKAGE)
# library_include_HEADERS = $(hh_sources)

MAINTAINERCLEANFILES = Makefile.in *~ .loT

CLEANFILES = herds_xml metadata_xml devaway_xml

herds_xml: herds_xml.cc libherdstat.la
	g++ -Wall -ggdb3 -DHAVE_CONFIG_H -I.. -c herds_xml.cc ; \
	../libtool --silent --tag=CXX --mode=link \
	g++ -Wall -ggdb3 -DHAVE_CONFIG_H -o herds_xml herds_xml.o libherdstat.la

metadata_xml: metadata_xml.cc libherdstat.la
	g++ -Wall -ggdb3 -DHAVE_CONFIG_H -I.. -c metadata_xml.cc ; \
	../libtool --silent --tag=CXX --mode=link \
	g++ -Wall -ggdb3 -DHAVE_CONFIG_H -o metadata_xml metadata_xml.o libherdstat.la

devaway_xml: devaway_xml.cc libherdstat.la
	g++ -Wall -ggdb3 -DHAVE_CONFIG_H -I.. -c devaway_xml.cc ; \
	../libtool --silent --tag=CXX --mode=link \
	g++ -Wall -ggdb3 -DHAVE_CONFIG_H -o devaway_xml devaway_xml.o libherdstat.la

userinfo_xml: userinfo_xml.cc libherdstat.la
	g++ -Wall -ggdb3 -DHAVE_CONFIG_H -I.. -c userinfo_xml.cc ; \
	../libtool --silent --tag=CXX --mode=link \
	g++ -Wall -ggdb3 -DHAVE_CONFIG_H -o userinfo_xml userinfo_xml.o libherdstat.la

srchtml: $(foreach f, $(libherdstat_la_SOURCES), $(f).html)
	@for x in $(SUBDIRS) ; do \
		cd ${PWD}/$${x} && $(MAKE) $@ ; \
	done

%.cc.html: %.cc
	@echo Generating $@
	@test -d $(top_srcdir)/html/herdstat || mkdir $(top_srcdir)/html/herdstat
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? -o ../html/herdstat/$@

%.hh.html: %.hh
	@echo Generating $@
	@test -d $(top_srcdir)/html/herdstat || mkdir $(top_srcdir)/html/herdstat
	@source-highlight -c ../../src.css -s cpp -f html -t4 -n -i $? -o ../html/herdstat/$@
