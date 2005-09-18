# $Id$

SUBDIRS = src

tests = string \
	glob \
	algo \
	herds.xml \
	devaway.xml \
	userinfo.xml \
	metadata.xml \
	versions \
	metadata \
	dev \
	herd \
	stats \
	away \
	find \
	pkg \
	which

TESTS = $(foreach f, $(tests), $(f)-test.sh)
TESTS_ENVIRONMENT = PORTDIR=portdir PORTDIR_OVERLAY=''

CLEANFILES = actual/* localstatedir/*cache*
MAINTAINERCLEANFILES = Makefile.in *~
EXTRA_DIST = $(TESTS) \
	     common.sh \
	     portdir \
	     expected \
	     localstatedir
