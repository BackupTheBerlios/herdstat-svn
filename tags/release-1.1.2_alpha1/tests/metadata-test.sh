#!/bin/bash
rv=0
source common.sh || exit 1
run_herdstat "${0}" "metadata handler" "-mnq foo"
