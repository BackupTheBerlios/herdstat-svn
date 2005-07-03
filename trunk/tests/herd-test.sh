#!/bin/sh
source common.sh || exit 1
run_herdstat "herd handler" "netmon" herd herd.expected
