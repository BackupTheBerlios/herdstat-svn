#!/bin/bash
source common.sh || exit 1
run_herdstat "${0}" "ebuild which" "-wq foo" || exit 1
run_herdstat "which-regex-test" "ebuild which (regex)" "-wqr foo" || exit 1
indent
