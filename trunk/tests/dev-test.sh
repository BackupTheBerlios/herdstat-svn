#!/bin/sh
source common.sh || exit 1
run_test "dev handler" "../src/herdstat -L . -d ka0ttic" \
    dev dev.expected
