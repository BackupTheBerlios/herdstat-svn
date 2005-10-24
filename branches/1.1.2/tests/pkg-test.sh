#!/bin/bash
source common.sh || exit 1
rm -f ${TEST_DATA}/localstatedir/*cache*

run_herdstat "pkg-herd-test" "pkg handler (herd)" "-pq fu" || exit 1
run_herdstat "pkg-dev-test"  "pkg handler (dev)"  "-pdq ka0ttic" || exit 1
run_herdstat "pkg-herd-nm-test" "pkg handler (herd/no-maintainer)" \
    "-pq fu --no-maintainer" || exit 1
run_herdstat "pkg-dev-nh-test" "pkg handler (dev/no-herd)" \
    "-pdq ka0ttic --no-herd" || exit 1

rm -f ${TEST_DATA}/localstatedir/*cache*
indent
