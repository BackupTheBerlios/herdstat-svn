#!/bin/bash
source common.sh || exit 1
run_herdstat "${0}" "metadata handler" "-mnq foo" || exit 1
run_herdstat "metadata-cat-test" "metadata handler (category)" \
    "-mq sys-libs" || exit 1
run_herdstat "metadata-regex-test" "metadata handler (regex)" "-mrq ." || exit 1
run_herdstat "metadata-ambiguous-test" "metadata handler (ambigious)" \
    "-mnq libfoo" "fail" || exit 1

origpwd=${PWD}
# output should be identical to "metadata-cat-test"
builtin cd ${PORTDIR}/sys-libs
run_herdstat "metadata-cat-test" "metadata handler (pwd category)" \
    "-mq" || exit 1
# output should be identical to "metadata-test"
builtin cd ${PORTDIR}/app-misc/foo
run_herdstat "${0}" "metadata handler (pwd)" "-mq" || exit 1
builtin cd ${origpwd}

indent
