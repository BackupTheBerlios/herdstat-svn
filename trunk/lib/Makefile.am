noinst_LTLIBRARIES = libutil.la
libutil_la_SOURCES = util_exceptions.hh \
		     util.hh util.cc \
		     string.hh string.cc \
		     timer.hh \
		     vars.hh vars.cc \
		     portage_misc.hh portage_misc.cc \
		     portage_config.hh

AM_CXXFLAGS = -pedantic -ansi -Wall
MAINTAINERCLEANFILES = Makefile.in *~ *.loT

srchtml:
	for x in $(libutil_la_SOURCES) ; do \
		gvim +TOhtml "+w! $${x}.html" +q! +q! +q $${x} ; \
	done
