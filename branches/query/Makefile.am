# $Id$

AUTOMAKE_OPTIONS = 1.8 dist-bzip2 no-dist-gzip

SUBDIRS = src doc

CLEANFILES = TODO
MAINTAINERCLEANFILES = aclocal.m4 \
		       configure \
		       Makefile.in \
		       config/* \
		       config.h.in \
		       stamp-h.in \
		       html/*/*.html \
		       *~

EXTRA_DIST = Makefile.am.common autogen.sh .todo bashcomp tests

check:
	cd tests && $(MAKE) $@

srchtml:
	@test -d ${PWD}/html || mkdir ${PWD}/html ; \
	for x in src ; do \
		if test -d ${PWD}/html/$${x} ; then \
			rm -fr ${PWD}/html/$${x}/* ; \
		else \
			mkdir ${PWD}/html/$${x} ; \
		fi ; \
		cd ${PWD}/$${x} && $(MAKE) $@ ; \
	done

TODO: .todo
	@test -x $(prefix)/bin/devtodo && devtodo all --TODO

distcheck-hook:
	@cp Makefile.am.common $(PACKAGE)-$(VERSION)/_build/

dist-hook: TODO

dist-sign: dist
	gpg --armour --detach-sign "$(PACKAGE)-$(VERSION).tar.bz2"

dist-upload: dist-sign
	echo -ne "user anonymous $(PACKAGE)\ncd incoming\nput $(PACKAGE)-$(VERSION).tar.bz2\nput $(PACKAGE)-$(VERSION).tar.bz2.asc\nbye" | \
		ftp -n ftp.berlios.de
