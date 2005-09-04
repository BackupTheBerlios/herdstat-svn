# $Id$

SUBDIRS = src

tests = string \
	glob \
	versions \
	metadata \
	metadata-regex \
	dev \
	herd \
	stats \
	away \
	find \
	find-regex \
	which

TESTS = $(foreach f, $(tests), $(f)-test.sh)
TESTS_ENVIRONMENT = PORTDIR=portdir PORTDIR_OVERLAY=''

CLEANFILES = actual/*
MAINTAINERCLEANFILES = Makefile.in *~ localstatedir/*cache*
EXTRA_DIST = $(TESTS) \
	     common.sh \
	     portdir \
	     expected \
	     localstatedir
