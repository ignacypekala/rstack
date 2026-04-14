#!/bin/bash
BG_RED_BLACK='\e[1;38;5;0;48;5;1m'
BG_GREEN_BLACK='\e[1;38;5;0;48;5;2m'
BG_YELLOW_BLACK='\e[1;38;5;0;48;5;3m'
BOLD_WHITE='\e[1;97m'
WHITE='\e[97m'
RESET='\e[0m'

batch_name=$1
test_name=$2
if [[ -v 3 ]]; then
    test_case=$3
fi

if [[ -v test_case ]]; then
    TEST_NAME="${BOLD_WHITE}tests_${batch_name}/${test_name} ${WHITE}(${2})${RESET}"
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

function show_diff() {
    diff -u --color "$1" "$2"
}

function end() {
    code=$1; shift
    show=$1; shift
    if [[ $show == diff ]]; then
        show_diff=true
        dif_file_a=$1; shift
        dif_file_b=$1; shift
    fi

    msg=$*
    if [[ $code == 0 ]]; then
        pass 
    elif [[ $code == 3 ]]; then
        warn
    else
        fail
    fi
    echo -e "${TEST_NAME} $msg"

    if [[ ( $code == 0 || $code == 3 ) && -s test.valgrind ]]; then
        echo "test.valgrind:"
        cat test.valgrind
        echo
    fi
    if [[ -s test.stderr ]]; then
        echo "test.stderr:"
        cat test.stderr
        echo
    fi

    if [[ $show == diff ]]; then
        show_diff $diff_file_a $diff_file_b
        echo
    elif [[ $show == make ]]; then
        cat test.make
        echo 
    fi

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
> test.stderr
> test.make

# compile
SECONDS=0
TEST_BATCH=$batch_name make test_$test_name -s &> test.make
compilation_code=$?
compilation_time=$SECONDS

if [[ $compilation_code != 0 ]]; then
    end 1 make "failed to compile"
fi

if [[ -s test.make ]]; then
    echo
    cat test.make
fi


# Craft a command for running the test
cmd=( 
    valgrind --track-origins=yes
    --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all
    --log-file="./test.valgrind" -q
    ./test_$test_name 
    
)

if [[ -v test_case ]]; then 
    case_file_format="./tests_$batch_name/$test_name/$test_case"
else
    case_file_format="./tests_$batch_name/$test_name"
fi
args_file="$case_file_format.args"
in_file="$case_file_format.in"
fout_file="$case_file_format.fout"
stdout_file="$case_file_format.stdout"

[[ -e $args_file ]] && cmd+=( $(cat "$args_file") )
[[ -e $in_file ]] || unset in_file
[[ -e $fout_file ]] || unset fout_file
[[ -e $stdout_file ]] || unset stdout_file

in_file="${in_file:-/dev/stdin}"

# run
SECONDS=0
LD_LIBRARY_PATH=. timeout 60s "${cmd[@]}" > test.stdout 2> test.stderr < $in_file
code=$?
execution_time=$SECONDS

exitcode=0;

if [[ $code == 0 ]]; then
    # Check stdout
    if [[ -v stdout_file ]] && ! diff -q "$stdout_file" test.stdout &> /dev/null
    then
        end 2 diff "$stdout_file" test.stdout "standard outputs differ"
    fi

    # Check file output
    if [[ -v fout_file ]] && ! diff -q "$fout_file" test.fout &> /dev/null
    then
        end 2 diff "$fout_file" test.fout "file outputs differ"
    fi

    # Check file output for named fout files
    shopt -s nullglob 
    for named_fout_file in ${case_file_format}_*.fout; do
        fout_name=${named_fout_file#${case_file_format}_}
        fout_name=${fout_name%.fout}

        if ! diff -q "$named_fout_file" test_$fout_name.fout &> /dev/null
        then
            end 2 diff "$named_fout_file" test_$fout_name.fout "named file outputs differ"
        fi
    done
    shopt -u nullglob 

    # Check valgrind report
    if [[ -s test.valgrind ]]; then
        end 3 no "valgrind reported errors"
    fi

    end 0 no "${execution_time}s"
    exit 0
else
    if [[ $code == 124 ]]; then
        end 3 no "program timed out after 60s"
    fi

    end 2 no "exited with code $code"
fi
