#!/bin/bash
source common.sh || exit 1
run_herdstat "${0}" "herd handler" "-q netmon" || exit 1
run_herdstat "herd-regex-test" "herd handler (regex)" "-nr ^n" || exit 1
run_herdstat "herd-all-test" "herd handler (all target)" "-n all" || exit 1
indent
