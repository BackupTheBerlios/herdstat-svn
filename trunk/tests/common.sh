. /sbin/functions.sh

run_herdstat() {
    local name="${1}" opts="${2}" actual="${3}" expected="${4}" rv=0
    ebegin "Testing ${name}"
    env PORTDIR_OVERLAY='' PORTDIR=${PWD}/portdir ../src/herdstat -L . \
	-H ./herds.xml -A ./devaway.html ${opts:-} -o ${actual} || rv=1
    diff ${expected} ${actual} &>/dev/null || rv=1
    eend ${rv}
    return ${rv}
}
