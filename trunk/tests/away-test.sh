#!/bin/sh
source common.sh || exit 1
run_test "away handler" "../src/herdstat -L . -a beu" \
    away away.expected
