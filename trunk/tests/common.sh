# pretty if possible
if [[ -f /sbin/functions.sh ]] ; then
    source /sbin/functions.sh
else
    function ebegin() { echo $* ; }
    function eend() { : ; }
fi

get_caller() {
    local x=${1##*/}
    echo ${x%-*}
}

indent() {
    echo -n "   "
}

run_test() {
    local caller="${1}" name="${2}" prog="${3}" opts="${4}" rv=0
    local actual="${srcdir:-.}/actual/${caller}" expected="${srcdir:-.}/expected/${caller}"

    [[ -d ${srcdir:-.}/actual ]] || mkdir ${srcdir:-.}/actual

    ebegin "Testing ${name}"

    # if 5th arg is passed, expected failure
    if [[ -z "${5}" ]] ; then
	${prog} ${opts} &> ${actual} || rv=1
    else
	${prog} ${opts} &> ${actual} && rv=1
    fi

    diff ${expected} ${actual}  || rv=1
    eend ${rv}
    return ${rv}
}

run_herdstat() {
    local lsd="${srcdir:-.}/localstatedir"
    run_test "$(get_caller ${1})" "${2}" "${srcdir:-.}/../src/herdstat" \
	"-T -L ${lsd} -A ${lsd}/devaway.xml -H ${lsd}/herds.xml ${3}" "${4}"
}
