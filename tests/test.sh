#!/bin/bash
# Configuration:
COLORS=yes
COMPILE=yes
VALGRIND=yes
VALGRIND_TRACK_ORIGINS=yes

if [[ $COLORS == yes ]]; then
    BG_RED_BLACK='\e[1;38;5;0;48;5;1m'
    BG_GREEN_BLACK='\e[1;38;5;0;48;5;2m'
    BG_YELLOW_BLACK='\e[1;38;5;0;48;5;3m'
    BOLD_WHITE='\e[1;97m'
    WHITE='\e[97m'
    RESET='\e[0m'
fi

batch_name=$1
test_name=$2
[[ -v 3 ]] && test_case=$3

if [[ -v test_case ]]; then
    TEST_NAME="${BOLD_WHITE}tests_${batch_name}/${test_name} ${WHITE}${test_case}${RESET} "
else
    TEST_NAME="${BOLD_WHITE}tests_${batch_name}/${test_name} ${RESET}"
fi
function pass() {
    echo -ne "${BG_GREEN_BLACK} PASS ${RESET} "
}
function fail() {
    echo -ne "${BG_RED_BLACK} FAIL ${RESET} "
}
function warn() {
    echo -ne "${BG_YELLOW_BLACK} WARN ${RESET} "
}

function run_diff() {
    if [[ $COLORS == yes ]]; then
        color=always
    else
        color=never
    fi

    diff -u --color=always "$1" "$2" > test.diff
    return $?
}

function end() {
    code=$1; shift
    show=$1; shift
    msg=$*

    if [[ $show == diff ]]; then
        cat test.diff
    fi

    if [[ ( $code == 0 || $code == 3 ) && -s test.valgrind ]]; then
        echo "test.valgrind:"
        cat test.valgrind
    fi

    if [[ $code == 0 ]]; then
        pass 
    elif [[ $code == 3 ]]; then
        warn
    else
        fail
    fi
    echo -e "${TEST_NAME}$msg"

    exit $code
}

function ensure_exists() {
    if ! [[ -e "$1" ]]; then
        end 1 no "$1 doesn't exist"
    fi
}
c_file="./tests_$batch_name/$test_name.c"
ensure_exists "$c_file"

# Clear output files
> test.fout
> test.stdout
> test.valgrind
> test.diff

executable="test_${test_name}_executable"

if [[ $COMPILE == yes ]]; then
    SECONDS=0
    TEST_BATCH=$batch_name make "$executable" -s
    compilation_code=$?
    compilation_time=$SECONDS
fi


if [[ $compilation_code != 0 ]]; then
    end 1 no "failed to compile"
fi

# Craft a command for running the test
if [[ $VALGRIND == yes ]]; then
    cmd=( 
        valgrind 
        --track-origins=$VALGRIND_TRACK_ORIGINS
        --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all
        -q
        --log-file="./test.valgrind" 
        "./$executable"
    
    )
else
    cmd=(
        "./$executable"
        )
fi

if [[ -v test_case ]]; then 
    case_file_format="./tests_$batch_name/$test_name/$test_case"
else
    case_file_format="./tests_$batch_name/$test_name"
fi
args_file="$case_file_format.args"
in_file="$case_file_format.in"
fout_file="$case_file_format.fout"
stdout_file="$case_file_format.stdout"

[[ -e "$args_file" ]] && cmd+=( $(cat "$args_file") )
[[ -e "$in_file" ]] || unset in_file
[[ -e "$fout_file" ]] || unset fout_file
[[ -e "$stdout_file" ]] || unset stdout_file

in_file="${in_file:-/dev/stdin}"
if [[ -v stdout_file ]]; then
    stdout=test.stdout
else
    stdout=/dev/stdout
fi


# run
SECONDS=0
LD_LIBRARY_PATH=. timeout 60s "${cmd[@]}" > "$stdout" < "$in_file"
code=$?
execution_time=$SECONDS

exitcode=0;

if [[ $code == 0 ]]; then
    # Check stdout
    if [[ -v stdout_file ]] && ! run_diff "$stdout_file" "$stdout"
    then
        end 2 diff "standard outputs differ"
    fi

    # Check file output
    if [[ -v fout_file ]] && ! run_diff "$fout_file" test.fout
    then
        end 2 diff "file outputs differ"
    fi

    # Check file output for named fout files
    shopt -s nullglob 
    for named_fout_file in ${case_file_format}_*.fout; do
        fout_name=${named_fout_file#${case_file_format}_}
        fout_name=${fout_name%.fout}

        if ! run_diff "$named_fout_file" test_$fout_name.fout
        then
            end 2 diff "named file outputs differ"
        fi
    done
    shopt -u nullglob 

    # Check valgrind report
    if [[ -s test.valgrind ]]; then
        end 3 no "valgrind reported errors"
    fi

    end 0 no "${execution_time}s (make: ${compilation_time}s)"
    exit 0
else
    if [[ $code == 124 ]]; then
        end 3 no "program timed out after 60s"
    fi

    end 2 no "exited with code $code"
fi
