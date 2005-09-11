#!/bin/bash
source common.sh || exit 1
run_test "$(get_caller ${0})" "devaway.xml parsing" \
    "${srcdir:-.}/src/devaway.xml-test" "${srcdir:-.}/localstatedir/devaway.xml"
