#!/bin/sh
source common.sh || exit 1
run_herdstat "${0}" "away handler" "-aq beu"
