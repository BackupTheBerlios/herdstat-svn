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

EXTRA_DIST = autogen.sh .todo bashcomp

TODO: .todo
	test -x $(prefix)/bin/devtodo && devtodo all --TODO

all: TODO
dist-hook: TODO

dist-sign: dist
	gpg --armour --detach-sign "$(PACKAGE)-$(VERSION).tar.bz2"

dist-upload: dist-sign
	echo -ne "user anonymous $(PACKAGE)\ncd incoming\nput $(PACKAGE)-$(VERSION).tar.bz2\nput $(PACKAGE)-$(VERSION).tar.bz2.asc\nbye" | \
		ftp -n ftp.berlios.de
