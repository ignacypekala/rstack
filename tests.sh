#!/bin/bash

passed=0
failed=0
warnings=0
misconfigured=0

function register() {
    case $1 in
        0)
            ((passed++))
            ;;
        1)
            ((misconfigured++))
            ;;
        2)
            ((failed++))
            ;;
        3)
            ((warnings++))
            ;;
    esac
}
for test in ./tests/*.c; do
    test_name=${test#./tests/}
    test_name=${test_name%.c}

    cmd=( ./test.sh $test_name )

    shopt -s nullglob # makes * return nothing when there aren't any matches
    input_files=( ./tests/$test_name/*.args )
    shopt -u nullglob 

    if [[ ${#input_files} > 0 ]]; then
        for input_file in ${input_files[@]}; do
            input_file_name=${input_file#./tests/$test_name/}
            input_name=${input_file_name%.args}

            ${cmd[@]} "$input_name"
            register $?
        done
    else
        "${cmd[@]}"
        register $?
    fi

done

RED="\e[31m"
GREEN='\e[32m'
YELLOW='\e[33m'
RESET='\e[0m' 
echo
echo -en "${RESET}Summary: "
echo -en "${GREEN}${passed}${RESET} passed"
if [[ $failed > 0 ]]; then
    echo -en ", ${RED}${failed}${RESET} failed"
fi
if [[ $warnings > 0 ]]; then
    echo -en ", ${YELLOW}${warnings}${RESET} raised warnings"
fi
if [[ $misconfigured > 0 ]]; then
    echo -e " and ${RED}${misconfigured}${RESET} failed to run."
else
    echo -e "." 
fi

