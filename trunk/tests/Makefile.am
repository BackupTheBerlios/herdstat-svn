# $Id$

SUBDIRS = foo

tests = versions metadata dev herd stats away
TESTS = $(foreach f, $(tests), $(f)-test.sh)

CLEANFILES = $(tests)
MAINTAINERCLEANFILES = Makefile.in *~
EXTRA_DIST = $(TESTS) \
	     common.sh \
	     herds.xml \
	     $(foreach f, $(tests), $(f).expected)
