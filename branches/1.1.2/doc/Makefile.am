doc_files = examples
noinst_DATA = $(foreach f, $(doc_files), $(f).txt)
man_MANS = herdstat.1

CLEANFILES = $(man_MANS) \
	     $(VERSION)/html/* \
	     examples.html

MAINTAINERCLEANFILES = Makefile.in *~
EXTRA_DIST = herdstat.1.in \
	     default.css \
	     herdstatrc.example \
	     $(noinst_DATA)

date  = date '+%b %Y'

html: $(foreach f, $(doc_files), $(f).html)

%.html: %.txt
	rst2html.py < $? > $@

herdstat.1: herdstat.1.in
	@SED@ -e "s,\@VERSION\@,$(VERSION),g" \
	      -e "s,\@PACKAGE\@,$(PACKAGE),g" \
	      -e "s,\@SYSCONFDIR\@,$(sysconfdir),g" $? > $@

dist-hook:
	@SED@ -i "s/\"[a-zA-Z]\{3\} [0-9]\{4\}\"/\"`$(date)`\"/" \
		$(distdir)/$(man_MANS:%=%.in)
