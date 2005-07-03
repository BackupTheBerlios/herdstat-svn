#!/bin/sh
source common.sh || exit 1
run_herdstat "stats handler" "" stats stats.expected
