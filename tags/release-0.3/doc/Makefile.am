man_MANS = herdstat.1

CLEANFILES = herdstat.1
MAINTAINERCLEANFILES = Makefile.in *~
EXTRA_DIST = herdstat.1.in

date  = date '+%b %Y'
dosed = @SED@ -e "s,\@VERSION\@,$(VERSION),g" \
	      -e "s,\@PACKAGE\@,$(PACKAGE),g"


herdstat.1: herdstat.1.in
	$(dosed) $? > $@

dist-hook:
	@SED@ -i "s/\"[a-zA-Z]\{3\} [0-9]\{4\}\"/\"`$(date)`\"/" \
		$(distdir)/$(man_MANS:%=%.in)
