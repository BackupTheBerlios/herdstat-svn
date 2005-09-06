#!/bin/sh
source common.sh || exit 1
run_test "$(get_caller ${0})" "metadata.xml parsing" \
    "${srcdir:-.}/src/metadata.xml-test" \
    "${srcdir:-.}/portdir/app-misc/foo/metadata.xml"