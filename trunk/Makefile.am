# $Id$

SUBDIRS = . doc lib src

CLEANFILES = TODO
MAINTAINERCLEANFILES = aclocal.m4 \
		       configure \
		       Makefile.in \
		       config/* \
		       config.h.in \
		       stamp-h.in \
		       *~

EXTRA_DIST = autogen.sh .todo bashcomp

srchtml:
	if test -d ${PWD}/html ; then \
		rm -fr ${PWD}/html/* ; \
	else \
		mkdir ${PWD}/html ; \
	fi
	for x in lib src ; do \
		cd ${PWD}/$${x} && $(MAKE) $@ ; \
		sleep 4 ; \
		test -d ${PWD}/html/$${x} || mkdir ${PWD}/html/$${x} ; \
		mv ${PWD}/$${x}/*.html ${PWD}/html/$${x} ; \
	done

TODO: .todo
	@test -x $(prefix)/bin/devtodo && devtodo all --TODO

dist-hook: TODO

dist-sign: dist
	gpg --armour --detach-sign "$(PACKAGE)-$(VERSION).tar.bz2"

dist-upload: dist-sign
	echo -ne "user anonymous $(PACKAGE)\ncd incoming\nput $(PACKAGE)-$(VERSION).tar.bz2\nput $(PACKAGE)-$(VERSION).tar.bz2.asc\nbye" | \
		ftp -n ftp.berlios.de
