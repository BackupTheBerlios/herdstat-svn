#!/bin/sh
source common.sh || exit 1
run_herdstat "away handler" "-a beu" away away.expected
