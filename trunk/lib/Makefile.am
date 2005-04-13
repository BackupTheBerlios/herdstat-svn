noinst_LTLIBRARIES = libutil.la
libutil_la_SOURCES = util_exceptions.hh \
		     util.hh util.cc \
		     string.hh string.cc \
		     timer.hh \
		     progress.hh \
		     file.hh file.cc \
		     vars.hh vars.cc \
		     portage_exceptions.hh \
		     portage_misc.hh portage_misc.cc \
		     portage_config.hh portage_config.cc \
		     portage_version.hh portage_version.cc

AM_CXXFLAGS = -pedantic -ansi -Wall
CLEANFILES = version-test
MAINTAINERCLEANFILES = Makefile.in *~ *.loT

version-test: $(noinst_LTLIBRARIES) version-test.cc
	$(CXX) $(AM_CXXFLAGS) -ggdb3 -c version-test.cc
	$(CXX) $(AM_CXXFLAGS) -o version-test version-test.o .libs/libutil.a \
		-Wl,--rpath -Wl,/usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5

srchtml:
	for x in $(libutil_la_SOURCES) ; do \
		source-highlight -c ../src.css -s cpp -f html -t4 -n -i $${x} \
			-o ../html/lib/$${x}.html ; \
	done
