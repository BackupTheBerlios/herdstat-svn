# $Id$

tests = versions \
	metadata \
	dev \
	herd \
	stats \
	away \
	find \
	pkg \
	which

TESTS = $(foreach f, $(tests), $(f)-test.sh)
TESTS_ENVIRONMENT = TEST_DATA=$(TEST_DATA) PORTDIR=$(TEST_DATA)/portdir PORTDIR_OVERLAY=''

CLEANFILES = actual/* localstatedir/*cache*
MAINTAINERCLEANFILES = Makefile.in *~
EXTRA_DIST = $(TESTS) common.sh expected
