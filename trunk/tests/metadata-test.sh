#!/bin/sh
rv=0
source common.sh || exit 1
run_herdstat "metadata handler" "-mn foo" metadata metadata.expected
