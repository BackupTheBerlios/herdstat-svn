# $Id$

SUBDIRS = . src doc

noinst_DATA = bashcomp

MAINTAINERCLEANFILES = aclocal.m4 \
		       configure \
		       Makefile.in \
		       config/config.guess \
		       config/ltmain.sh \
		       config/config.sub \
		       config/depcomp \
		       config.h.in \
		       stamp-h.in \
		       *~

EXTRA_DIST = autogen.sh $(noinst_DATA)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(datadir)/bash-completion
	$(INSTALL_DATA) bashcomp $(DESTDIR)$(datadir)/bash-completion/@PACKAGE@
