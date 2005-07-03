# $Id$

tests = versions \
	metadata \
	metadata-regex \
	dev \
	herd \
	stats \
	away \
	find \
	find-regex

TESTS = $(foreach f, $(tests), $(f)-test.sh)

CLEANFILES = $(tests)
MAINTAINERCLEANFILES = Makefile.in *~
EXTRA_DIST = $(TESTS) \
	     common.sh \
	     herds.xml \
	     devaway.html \
	     $(foreach f, $(tests), $(f).expected) \
	     portdir
