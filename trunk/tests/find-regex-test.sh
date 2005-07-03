#!/bin/sh
source common.sh || exit 1
run_herdstat "find handler (regex)" "-fr ^foo" find-regex find-regex.expected
