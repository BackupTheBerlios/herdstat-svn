#!/bin/sh
source common.sh || exit 1
run_test "away handler" "../src/herdstat -H ./herds.xml -A ./devaway.html -a beu" \
    away away.expected
