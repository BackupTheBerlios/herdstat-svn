noinst_LTLIBRARIES = libutil.la
libutil_la_SOURCES = util_exceptions.hh \
		     util.hh \
		     string.hh string.cc \
		     misc.hh misc.cc \
		     regex.hh regex.cc \
		     timer.hh \
		     progress.hh \
		     file.hh file.cc \
		     vars.hh vars.cc \
		     cache.hh

# if HAVE_LIBZ
# libutil_la_SOURCES += zlib.hh zlib.cc
# endif

libutil_la_SOURCES += portage_exceptions.hh \
		      portage_misc.hh portage_misc.cc \
		      portage_config.hh portage_config.cc \
		      portage_version.hh portage_version.cc \
		      portage_find.hh portage_find.cc

LIBS = @LIBS@ @GLIBMM_LIBS@
AM_CXXFLAGS = -pedantic -ansi -Wall -Wno-long-long @GLIBMM_CFLAGS@

CLEANFILES = version-test
MAINTAINERCLEANFILES = Makefile.in *~ *.loT

version-test: $(noinst_LTLIBRARIES) version-test.cc
	$(CXX) $(AM_CXXFLAGS) -ggdb3 -I.. -c version-test.cc
	$(CXX) $(AM_CXXFLAGS) -o version-test version-test.o .libs/libutil.a $(LIBS)

srchtml: $(foreach f, $(libutil_la_SOURCES), $(f).html)
%.cc.html: %.cc
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/lib/$@
%.hh.html: %.hh
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/lib/$@
