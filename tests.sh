#!/bin/bash
for test in ./tests/*.c; do
    test_name=${test#./tests/}
    test_name=${test_name%.c}
    for input_file in ./tests/$test_name/*.in; do
        cmd=( ./test.sh $test_name $input_file )
        echo ${cmd[@]}
        "${cmd[@]}"
    done
done
