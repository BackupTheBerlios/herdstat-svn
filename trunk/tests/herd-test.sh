#!/bin/sh
source common.sh || exit 1
run_test "herd handler" "../src/herdstat -L . netmon" \
    herd herd.expected
