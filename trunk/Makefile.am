# $Id$

SUBDIRS = . src doc

CLEANFILES = TODO
MAINTAINERCLEANFILES = aclocal.m4 \
		       configure \
		       Makefile.in \
		       config/* \
		       config.h.in \
		       stamp-h.in \
		       *~

EXTRA_DIST = autogen.sh .todo

TODO: .todo
	@if test -x /usr/bin/devtodo ; then \
		devtodo all --TODO  ; \
	fi

all: TODO
dist-hook: TODO
