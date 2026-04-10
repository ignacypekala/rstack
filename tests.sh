#!/bin/bash

passes=0
fails=0
warnings=0
misconfigured=0
function count() {
    case $1 in
        0)
            ((passes++))
            ;;
        1)
            ((misconfigured++))
            ;;
        2)
            ((fails++))
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
    input_files=( ./tests/$test_name/*.in )
    shopt -u nullglob 

if [[ ${#input_files} > 0 ]]; then
    for input_file in ./tests/$test_name/*.in; do
        input_file_name=${input_file#./tests/$test_name/}
        input_name=${input_file_name%.in}

        ${cmd[@]} "$input_name"
        count $?
        echo
    done
else
    "${cmd[@]}"
    count $?
    echo
fi

done

RED="\e[31m"
GREEN='\e[32m'
YELLOW='\e[33m'
RESET='\e[0m' 
echo -en "${RESET}Summary: ${GREEN}${passes}${RESET} passed, "
echo -en "${RED}${fails}${RESET} failed, "
echo -en "${YELLOW}${warnings}${RESET} exited with warnings "
echo -e "and ${misconfigured} failed to run."
