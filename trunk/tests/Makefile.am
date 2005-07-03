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
CLEANFILES = $(foreach f, $(tests), actual/$(f))
MAINTAINERCLEANFILES = Makefile.in *~
EXTRA_DIST = $(TESTS) \
	     common.sh \
	     herds.xml \
	     devaway.html \
	     portdir \
	     expected
