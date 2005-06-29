#!/bin/sh
source common.sh || exit 1
run_test "dev handler" "../src/herdstat -H ./herds.xml -d ka0ttic" dev dev.expected
