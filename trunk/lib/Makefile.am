noinst_LTLIBRARIES = libutil.la
libutil_la_SOURCES = util_exceptions.hh \
		     util.hh util.cc \
		     string.hh string.cc \
		     timer.hh \
		     progress.hh \
		     vars.hh vars.cc \
		     file.hh file.cc \
		     portage_misc.hh portage_misc.cc \
		     portage_config.hh

AM_CXXFLAGS = -pedantic -ansi -Wall
MAINTAINERCLEANFILES = Makefile.in *~ *.loT

srchtml:
	for x in $(libutil_la_SOURCES) ; do \
		source-highlight -c ../src.css -s cpp -f html -t4 -n -i $${x} \
			-o ../html/lib/$${x}.html ; \
	done
