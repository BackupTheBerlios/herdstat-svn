#!/bin/bash
source common.sh || exit 1
run_test "$(get_caller ${0})" "algorithms" \
    "${srcdir:-.}/src/algo-test" "" || exit 1
indent
