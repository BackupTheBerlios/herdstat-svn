#!/bin/sh
source common.sh || exit 1
run_test "stats handler" "../src/herdstat -L ." \
    stats stats.expected
