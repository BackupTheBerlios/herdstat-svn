# $Id$

include $(top_builddir)/Makefile.am.common

SUBDIRS = io action

bin_PROGRAMS = herdstat

herdstat_SOURCES = \
	exceptions.hh \
	handler_map.hh \
	options.hh options.cc \
	common.hh common.cc \
	rc.hh rc.cc \
	xmlinit.hh xmlinit.cc \
	formatter.hh formatter.cc \
	cache.hh cache.cc \
	package_cache.hh package_cache.cc \
	metadata_cache.hh metadata_cache.cc \
	overlay_display.hh overlay_display.cc \
	fields.hh \
	query_base.hh \
	query.hh query.cc \
	query_results.hh \
	herdstat.cc

herdstat_LDADD = \
	io/libio.la \
	action/libaction.la \
	$(libherdstat_LIBS)

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

install-data-local: $(foreach f, $(symlinks), install-symlink-$(f))

install-symlink-%:
	ln -snf $(DESTDIR)$(bindir)/herdstat \
		$(DESTDIR)$(bindir)/herdstat-$*

srchtml: $(foreach f, $(herdstat_SOURCES), $(f).html)
%.cc.html: %.cc
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/src/$@
%.hh.html: %.hh
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/src/$@
