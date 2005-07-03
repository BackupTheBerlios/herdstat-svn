#!/bin/sh
source common.sh || exit 1
run_herdstat "dev handler" "-d ka0ttic" dev dev.expected
