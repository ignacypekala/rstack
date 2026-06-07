#!/bin/bash
# Configuration:
COLORS=yes

batch_name=$1
batch_dir=./tests_${batch_name}

if ! [[ -d $batch_dir ]]; then
    echo "$batch_dir doesn't exist or isn't a directory"
fi

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
shopt -s nullglob # makes * return nothing when there aren't any matches
for test in $batch_dir/*.c; do
    test_name=${test#$batch_dir/}
    test_name=${test_name%.c}

    cmd=( ./test.sh "$batch_name" "$test_name" )

    case_files=( $batch_dir/$test_name/*.{args,in,stdout,fout} )

    declare -A handled

    if [[ ${#case_files} > 0 ]]; then
        for case_file in ${case_files[@]}; do
            case_name=${case_file#$batch_dir/$test_name/}
            case_name=${case_name%.*}
            case_name=${case_name%_*}

            # case hasn't been handled yet
            if [[ -z "${handled[$case_name]}" ]]; then
                # mark as handled
                handled[$case_name]=1

                "${cmd[@]}" "$case_name"
                register $?
            fi
        done
    else
        "${cmd[@]}"
        register $?
    fi

done
shopt -u nullglob 

if [[ $COLORS == yes ]]; then
    RED="\e[31m"
    GREEN='\e[32m'
    YELLOW='\e[33m'
    RESET='\e[0m' 
fi
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

