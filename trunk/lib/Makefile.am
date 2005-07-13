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
		     glob.hh \
		     cache.hh\
		     portage_exceptions.hh \
		     portage_misc.hh portage_misc.cc \
		     portage_config.hh portage_config.cc \
		     portage_version.hh portage_version.cc \
		     portage_find.hh portage_find.cc

LIBS = @LIBS@ @TERMCAP_LIBS@ @GLIBMM_LIBS@
AM_CXXFLAGS = -pedantic -ansi -Wall -Werror @GLIBMM_CFLAGS@

MAINTAINERCLEANFILES = Makefile.in *~ *.loT

srchtml: $(foreach f, $(libutil_la_SOURCES), $(f).html)
%.cc.html: %.cc
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/lib/$@
%.hh.html: %.hh
	@echo Generating $@
	@source-highlight -c ../src.css -s cpp -f html -t4 -n -i $? -o ../html/lib/$@
