include $(top_builddir)/Makefile.am.common

noinst_PROGRAMS = string-test \
		  glob-test \
		  algo-test \
		  herds.xml-test \
		  devaway.xml-test \
		  userinfo.xml-test \
		  metadata.xml-test

string_test_SOURCES = string-test.cc
string_test_LDADD = $(top_builddir)/herdstat/libherdstat.la
glob_test_SOURCES = glob-test.cc
glob_test_LDADD = $(top_builddir)/herdstat/libherdstat.la
algo_test_SOURCES = algo-test.cc
algo_test_LDADD = $(top_builddir)/herdstat/libherdstat.la
herds_xml_test_SOURCES = herds.xml-test.cc
herds_xml_test_LDADD = $(top_builddir)/herdstat/libherdstat.la
devaway_xml_test_SOURCES = devaway.xml-test.cc
devaway_xml_test_LDADD = $(top_builddir)/herdstat/libherdstat.la
userinfo_xml_test_SOURCES = userinfo.xml-test.cc
userinfo_xml_test_LDADD = $(top_builddir)/herdstat/libherdstat.la
metadata_xml_test_SOURCES = metadata.xml-test.cc
metadata_xml_test_LDADD = $(top_builddir)/herdstat/libherdstat.la

MAINTAINERCLEANFILES = Makefile.in *~ .loT
