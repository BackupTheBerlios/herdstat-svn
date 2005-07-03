#!/bin/sh
source common.sh || exit 1
run_herdstat "metadata handler (regex)" "-mr ." metadata-regex metadata-regex.expected
