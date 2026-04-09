#!/bin/bash
for test in ./tests/*.c; do
    test_name=${test#./tests/}
    test_name=${test_name%.c}
    
    cmd=( ./test.sh $test_name )

    shopt -s nullglob # makes * return nothing when there aren't any matches
    input_files=( ./tests/$test_name/*.in )
    shopt -u nullglob 

    if [[ ${#input_files} > 0 ]]; then
        for input_file in ./tests/$test_name/*.in; do
            echo ${cmd[@]} $input_file 
            (${cmd[@]} $input_file)
            echo
        done
    else
        echo ${cmd[@]}
        "${cmd[@]}"
        echo
    fi
done
