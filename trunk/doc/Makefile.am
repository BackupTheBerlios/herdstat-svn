doc_files = examples
noinst_DATA = $(foreach f, $(doc_files), $(f).txt)
man_MANS = herdstat.1

CLEANFILES = \
	$(man_MANS) \
	$(VERSION)/html/* \
	examples.html \
	Doxyfile

MAINTAINERCLEANFILES = Makefile.in *~
EXTRA_DIST = \
	herdstat.1.in \
	default.css \
	herdstatrc.example \
	$(noinst_DATA) \
	Doxyfile.in

date  = date '+%b %Y'

dosed = @SED@ -e "s,\@VERSION\@,$(VERSION),g" \
	      -e "s,\@PACKAGE\@,$(PACKAGE),g" \
	      -e "s,\@SYSCONFDIR\@,$(sysconfdir),g" 

install-data-local: install-man $(foreach f, $(symlinks), install-symlink-$(f))

install-symlink-%:
	ln -snf \
		$(DESTDIR)$(mandir)/man1/herdstat.1 \
		$(DESTDIR)$(mandir)/man1/herdstat-$*.1

html: $(foreach f, $(doc_files), $(f).html)

%.html: %.txt
	rst2html.py < $? > $@

herdstat.1: herdstat.1.in
	$(dosed) $? > $@

Doxyfile: Doxyfile.in
	$(dosed) $? > $@

doxygen: Doxyfile
	doxygen

dist-hook:
	@SED@ -i "s/\"[a-zA-Z]\{3\} [0-9]\{4\}\"/\"`$(date)`\"/" \
		$(distdir)/$(man_MANS:%=%.in)
