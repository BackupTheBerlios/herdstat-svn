#!/bin/sh
source common.sh || exit 1
run_test "herd handler" "../src/herdstat -H ./herds.xml netmon" herd herd.expected
