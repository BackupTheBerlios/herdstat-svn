#!/bin/bash
source common.sh || exit 1
run_herdstat "${0}" "version sorting" "--versions -q foo" || exit 1
indent
