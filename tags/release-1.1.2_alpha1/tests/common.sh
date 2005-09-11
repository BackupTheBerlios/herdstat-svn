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

run_test() {
    local caller="${1}" name="${2}" prog="${3}" opts="${4}" rv=0
    local actual="${srcdir:-.}/actual/${caller}" expected="${srcdir:-.}/expected/${caller}"

    [[ -d ${srcdir:-.}/actual ]] || mkdir ${srcdir:-.}/actual

    ebegin "Testing ${name}"
    ${prog} ${opts} > ${actual} || rv=1
    diff ${expected} ${actual}  || rv=1
    eend ${rv}
    echo -n "   "
    return ${rv}
}

run_herdstat() {
    local lsd="${srcdir:-.}/localstatedir"
    run_test "$(get_caller ${1})" "${2}" "${srcdir:-.}/../src/herdstat" \
	"-L ${lsd} -A ${lsd}/devaway.xml -H ${lsd}/herds.xml ${3}"
}
