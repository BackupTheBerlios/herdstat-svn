#!/bin/bash
source common.sh || exit 1
rm -f ${srcdir}/expected/which*
echo "${TEST_DATA}/portdir/app-misc/foo/foo-1.10.20050629-r1.ebuild" > \
    ${srcdir}/expected/which

for x in portdir/app-lala/foomatic/foomatic-1000.ebuild \
    portdir/app-misc/foo/foo-1.10.20050629-r1.ebuild \
    portdir/media-libs/libfoo/libfoo-0.98_rc2.ebuild \
    portdir/sys-libs/libfoo/libfoo-1.9.ebuild ; do
    echo "${TEST_DATA}/${x}" >> ${srcdir}/expected/which-regex
done

run_herdstat "${0}" "which handler" "-wq foo" || exit 1
run_herdstat "which-regex-test" "which handler (regex)" "-wqr foo" || exit 1

rm -f ${srcdir}/expected/which*

indent
