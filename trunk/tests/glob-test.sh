#!/bin/sh
source common.sh || exit 1
run_test "$(get_caller ${0})" "pattern matching" \
    "${srcdir:-.}/src/glob-test" ""
