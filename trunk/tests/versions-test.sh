#!/bin/sh
rv=0
source common.sh || exit 1
builtin pushd foo &>/dev/null
run_test "version sorting" "../../src/herdstat -L .. --versions --quiet" \
    ../versions ../versions.expected || rv=1
builtin popd &>/dev/null
exit ${rv}
