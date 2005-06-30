#!/bin/sh
source common.sh || exit 1
run_test "stats handler" "../src/herdstat -H ./herds.xml -A ./devaway.html" \
    stats stats.expected
