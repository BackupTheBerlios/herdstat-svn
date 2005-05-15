# $Id: Makefile.am 234 2005-04-19 12:34:46Z ka0ttic $

SUBDIRS = doc lib src

CLEANFILES = TODO
MAINTAINERCLEANFILES = aclocal.m4 \
		       configure \
		       Makefile.in \
		       config/* \
		       config.h.in \
		       stamp-h.in \
		       html/*/*.html \
		       *~

EXTRA_DIST = autogen.sh .todo bashcomp

srchtml:
	test -d ${PWD}/html || mkdir ${PWD}/html ; \
	for x in lib src ; do \
		if test -d ${PWD}/html/$${x} ; then \
			rm -fr ${PWD}/html/$${x}/* ; \
		else \
			mkdir ${PWD}/html/$${x} ; \
		fi ; \
		cd ${PWD}/$${x} && $(MAKE) $@ ; \
	done

TODO: .todo
	@test -x $(prefix)/bin/devtodo && devtodo all --TODO

dist-hook: TODO

dist-sign: dist
	gpg --armour --detach-sign "$(PACKAGE)-$(VERSION).tar.bz2"

dist-upload: dist-sign
	echo -ne "user anonymous $(PACKAGE)\ncd incoming\nput $(PACKAGE)-$(VERSION).tar.bz2\nput $(PACKAGE)-$(VERSION).tar.bz2.asc\nbye" | \
		ftp -n ftp.berlios.de
