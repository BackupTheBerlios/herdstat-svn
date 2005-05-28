doc_files = examples
noinst_DATA = $(foreach f, $(doc_files), $(f).txt)

man_MANS = herdstat.1

CLEANFILES = herdstat.1 $(foreach f, $(doc_files), $(f).html)
MAINTAINERCLEANFILES = Makefile.in *~
EXTRA_DIST = herdstat.1.in default.css $(noinst_DATA)

date  = date '+%b %Y'

html: $(foreach f, $(doc_files), $(f).html)

%.html: %.txt
	rst2html.py < $? > $@

herdstat.1: herdstat.1.in
	@SED@ -e "s,\@VERSION\@,$(VERSION),g" \
	      -e "s,\@PACKAGE\@,$(PACKAGE),g" $? > $@

dist-hook:
	@SED@ -i "s/\"[a-zA-Z]\{3\} [0-9]\{4\}\"/\"`$(date)`\"/" \
		$(distdir)/$(man_MANS:%=%.in)
