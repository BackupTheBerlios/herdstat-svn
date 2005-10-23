#!/bin/bash
source common.sh || exit 1
run_herdstat "${0}" "keyword handler" "-qk sys-libs/libfoo" || exit 1
indent
