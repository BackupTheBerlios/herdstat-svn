# pretty if possible
if [[ -f /sbin/functions.sh ]] ; then
    source /sbin/functions.sh
else
    function ebegin() { echo $* ; }
    function eend() { : ; }
fi

run_test() {
    local caller="${1}" name="${2}" prog="${3}" opts="${4}" rv=0
    local actual="${PWD}/actual/${caller}" expected="${PWD}/expected/${caller}"

    ebegin "Testing ${name}"
    ${prog} ${opts} > ${actual} || rv=1
    diff ${expected} ${actual}  || rv=1
    eend ${rv}
    return ${rv}
}

run_herdstat() {
    run_test "${1}" "${2}" \
	"env PORTDIR_OVERLAY='' PORTDIR=${PWD}/portdir ../src/herdstat" \
	"${3}"
}

#run_herdstat() {
#    local name="${1}" opts="${2}" actual="${3}" expected="${4}" rv=0

#    ebegin "Testing ${name}"
#    
#    env PORTDIR_OVERLAY='' PORTDIR=${PWD}/portdir ../src/herdstat -L . \
#        -H ./herds.xml -A ./devaway.html ${opts:-} -o ${actual} || rv=1
#    
#    diff ${expected} ${actual} &>/dev/null || rv=1
#    
#    eend ${rv}
#    return ${rv}
#}
