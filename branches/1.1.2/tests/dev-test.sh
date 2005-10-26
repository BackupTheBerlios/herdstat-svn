#!/bin/bash
source common.sh || exit 1
run_herdstat "${0}" "dev handler" "-dq ka0ttic" || exit 1
run_herdstat "dev-regex-test" "dev handler (regex)" "-dnE ^(y|s)" || exit 1
#run_herdstat "dev-field-test" "dev handler (field)" \
#    "-U ${TEST_DATA}/localstatedir/userinfo.xml -dn --field=name,^s" || exit 1
run_herdstat "dev-all-test" "dev handler (all target)" "-dn all" || exit 1
indent
