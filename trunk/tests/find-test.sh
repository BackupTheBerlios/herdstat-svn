#!/bin/sh
source common.sh || exit 1
run_herdstat "find handler" "-f foo" find find.expected
