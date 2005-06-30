run_test() {
    local label="${1}" hs="${2}" actual="${3}" expected="${4}"
    printf ">>> Testing %s..." "${label}"
    ${hs} -o ${actual} || return 1
    diff ${expected} ${actual} &>/dev/null
}
