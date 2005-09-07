#!/bin/sh
source common.sh || exit 1
run_test "$(get_caller ${0})" "userinfo.xml parsing" \
    "${srcdir:-.}/src/userinfo.xml-test" \
    "${srcdir:-.}/localstatedir/userinfo.xml ka0ttic"
