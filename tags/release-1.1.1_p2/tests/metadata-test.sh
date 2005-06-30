#!/bin/sh
rv=0
source common.sh || exit 1
builtin pushd foo &>/dev/null
run_test "metadata handler" "../../src/herdstat -mn -H ../herds.xml -A ../devaway.html" \
    ../metadata ../metadata.expected || rv=1
builtin popd &>/dev/null
exit ${rv}
