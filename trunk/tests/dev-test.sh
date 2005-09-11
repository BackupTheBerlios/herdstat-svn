#!/bin/bash
source common.sh || exit 1
run_herdstat "${0}" "dev handler" "-dq ka0ttic"
