#!/bin/bash

BG_RED_BLACK='\e[1;38;5;0;48;5;1m'
BG_GREEN_BLACK='\e[1;38;5;0;48;5;2m'
BG_YELLOW_BLACK='\e[1;38;5;0;48;5;3m'
RESET='\e[0m'
function success() {
    echo -e "${BG_GREEN_BLACK} SUCCESS ${RESET} $*"
}
function fail() {
    echo -e "${BG_RED_BLACK} FAIL ${RESET} $*"
}
function warn() {
    echo -e "${BG_YELLOW_BLACK} WARN ${RESET} $*"
}

name=$1

make test_$name -s

if [[ $2 == no ]]; then
    cmd=( ./test_$name )
else
    cmd=( 
        valgrind 
        --track-origins=yes --leak-check=full 
        -q
        ./test_$name
    )
fi

if [[ $2 != "" ]]; then
    input_file=$2
    input_file_name=${input_file#./tests/$name/}

    cmd+=( $(cat $input_file) )
    output_file=${input_file%.in}.out
fi

# echo ${cmd[@]}
"${cmd[@]}" > test.out

code=$?
if [[ $code == 0 ]]; then
    if [[ -v output_file ]] && ! diff -q $output_file test.out > /dev/null; then
        fail The outputs differ:
        diff -u --color $output_file test.out 
        exit 2
    else
        # if [[ $code == 3 ]]; then
        #     warn "Valgrind reported errors in $name"
        #     exit 3
        # fi
        if [[ -v input_file_name ]]; then
            success $name \($input_file_name\)
        else
            success $name
        fi
        exit 0
    fi
else
    fail Exited with code $code
    exit 1
fi


