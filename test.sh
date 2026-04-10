#!/bin/bash
# Usage
# For tests without arguments:
# ./tests.sh TEST_NAME
# In this case ./tests/TEST_NAME.(fout|stdout) are analyzed.
#
# For tests with arguments in ./tests/TEST_NAME/INPUT_NAME.args:
# ./tests.sh TEST_NAME INPUT_NAME
# In this case ./tests/TEST_NAME/INPUT_NAME.(fout|stdout) are analyzed.

BG_RED_BLACK='\e[1;38;5;0;48;5;1m'
BG_GREEN_BLACK='\e[1;38;5;0;48;5;2m'
BG_YELLOW_BLACK='\e[1;38;5;0;48;5;3m'
BOLD_WHITE='\e[1;97m'
WHITE='\e[97m'
RESET='\e[0m'

name=$1
if [[ -v 2 ]]; then
    TEST_NAME="${BOLD_WHITE}${name} ${WHITE}(${2}):${RESET}"
else
    TEST_NAME="${BOLD_WHITE}${name}:${RESET}"
fi
function pass() {
    echo -e "${BG_GREEN_BLACK} PASS ${RESET} ${TEST_NAME} $*"
}
function fail() {
    echo -e "${BG_RED_BLACK} FAIL ${RESET} ${TEST_NAME} $*"
}
function warn() {
    echo -e "${BG_YELLOW_BLACK} WARN ${RESET} ${TEST_NAME} $*"
}

function ensure_exists() {
    if ! [[ -e "$1" ]]; then
        echo "$1 doesn't exist"
        exit 1
    fi
}
c_file="./tests/$name.c"
ensure_exists "$c_file"

make test_$name -s

# Clear output files
> test.fout
> test.stdout
> test.valgrind

# Craft a command for running the test
cmd=( 
    valgrind --track-origins=yes --leak-check=full
    --log-file="./test.valgrind" -q
    ./test_$name 
    
)

# If INPUT_NAME was given
if [[ -v 2 ]]; then 
    input_name="$2"
    args_file="./tests/$name/${input_name}.args"
    ensure_exists "$args_file"

    cmd+=( $(cat "$args_file") )

    fout_file="./tests/$name/${input_name}.fout"
    stdout_file="./tests/$name/${input_name}.stdout"
else
    fout_file="./tests/${name}.fout"
    stdout_file="./tests/${name}.stdout"
fi

if ! [[ -e "$fout_file" ]]; then
    unset fout_file
fi
if ! [[ -e "$stdout_file" ]]; then
    unset stdout_file
fi

# Run and pipe to test.stdout
"${cmd[@]}" > test.stdout
code=$?
if [[ $code == 0 ]]; then
    # Check standard output
    if [[ -v stdout_file ]] && ! diff -q "$stdout_file" test.stdout > /dev/null
    then
        fail The standard outputs differ:
        diff -u --color "$stdout_file" test.stdout
        exit 2
    fi
    # Check file output
    if [[ -v fout_file ]] && ! diff -q "$fout_file" test.fout > /dev/null
    then
        fail The file outputs differ:
        diff -u --color "$fout_file" test.fout 
        exit 2
    fi
    # Check valgrind report
    if [[ -s test.valgrind ]]; then
        warn "Valgrind reported errors in $name:"
        cat test.valgrind
        exit 3
    fi

    if [[ -v input_file_name ]]; then
        pass $name \($input_file_name\)
    else
        pass $name
    fi
    exit 0
else
    fail Exited with code $code
    exit 2
fi
