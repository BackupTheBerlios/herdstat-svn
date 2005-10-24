#!/bin/bash
source common.sh || exit 1
run_herdstat "${0}" "away handler" "-aq beu" || exit 1
run_herdstat "away-regex-test" "away handler (regex)" "-anr ^s" || exit 1
run_herdstat "away-all-test" "away handler (all target)" "-an all" || exit 1
indent
