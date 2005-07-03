#!/bin/sh
source common.sh || exit 1
run_herdstat "version sorting" "--versions --quiet foo" versions versions.expected
