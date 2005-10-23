#!/bin/bash
source common.sh || exit 1
run_herdstat "${0}" "metadata handler" "-mnq foo" || exit 1
run_herdstat "metadata-cat-test" "metadata handler (category)" \
    "-mq sys-libs" || exit 1
run_herdstat "metadata-regex-test" "metadata handler (regex)" "-mrq ." || exit 1
run_herdstat "metadata-ambiguous-test" "metadata handler (ambigious)" \
    "-mnq libfoo" "fail" || exit 1
indent
