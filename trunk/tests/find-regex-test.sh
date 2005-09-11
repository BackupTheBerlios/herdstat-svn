#!/bin/bash
source common.sh || exit 1
run_herdstat "${0}" "find handler (regex)" "-frq ^foo"
