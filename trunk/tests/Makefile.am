# $Id$
tests = versions \
	metadata \
	metadata-regex \
	dev \
	herd \
	stats \
	away \
	find \
	find-regex \
	which

if BUILD_TESTS
SUBDIRS = src
tests += string
endif


TESTS = $(foreach f, $(tests), $(f)-test.sh)
TESTS_ENVIRONMENT = PORTDIR=portdir PORTDIR_OVERLAY=''

CLEANFILES = actual/*
MAINTAINERCLEANFILES = Makefile.in *~
EXTRA_DIST = $(TESTS) \
	     common.sh \
	     portdir \
	     expected \
	     localstatedir
