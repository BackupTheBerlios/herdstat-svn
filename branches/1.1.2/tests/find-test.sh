#!/bin/bash
source common.sh || exit 1
run_herdstat "${0}" "find handler" "-fq foo" || exit 1
run_herdstat "find-regex-test" "find handler (regex)" "-frq ^foo" || exit 1
run_herdstat "find-nonexistent-test" "find handler (nonexistent)" \
    "-frq non-existent" "fail" || exit 1
indent
