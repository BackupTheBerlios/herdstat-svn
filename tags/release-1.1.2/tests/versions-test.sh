#!/bin/bash
source common.sh || exit 1
run_herdstat "${0}" "versions handler" "--versions -q foo" || exit 1
indent
