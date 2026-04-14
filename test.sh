#!/bin/bash
# Usage
# For tests without arguments:
# ./tests.sh TEST_NAME
# In this case $TEST_DIR/TEST_NAME.(fout|stdout) are analyzed.
#
# For tests with arguments in $TEST_DIR/TEST_NAME/INPUT_NAME.args:
# ./tests.sh TEST_NAME INPUT_NAME
# In this case $TEST_DIR/TEST_NAME/INPUT_NAME.(fout|stdout) are analyzed.

BG_RED_BLACK='\e[1;38;5;0;48;5;1m'
BG_GREEN_BLACK='\e[1;38;5;0;48;5;2m'
BG_YELLOW_BLACK='\e[1;38;5;0;48;5;3m'
BOLD_WHITE='\e[1;97m'
WHITE='\e[97m'
RESET='\e[0m'

TEST_DIR="./tests"

name=$1
if [[ -v 2 ]]; then
    TEST_NAME="${BOLD_WHITE}${name} ${WHITE}(${2})${RESET}"
else
    TEST_NAME="${BOLD_WHITE}${name}${RESET}"
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

function show_diff() {
    diff -u --color "$1" "$2"
}

function ensure_exists() {
    if ! [[ -e "$1" ]]; then
        fail "$1 doesn't exist"
        exit 1
    fi
}
c_file="$TEST_DIR/$name.c"
ensure_exists "$c_file"

# Clear output files
> test.fout
> test.stdout
> test.valgrind
> test.stderr
> test.make

SECONDS=0
make test_$name -s &> test.make
compilation_code=$?
compilation_time=$SECONDS

if [[ $compilation_code != 0 ]]; then
    fail "failed to compile:"
    cat test.make
    echo
    exit 1
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
    ./test_$name 
    
)

# If INPUT_NAME was given
if [[ -v 2 ]]; then 
    input_name="$2"
    args_file="$TEST_DIR/$name/${input_name}.args"
    ensure_exists "$args_file"

    cmd+=( $(cat "$args_file") )

    fout_file="$TEST_DIR/$name/${input_name}.fout"
    stdout_file="$TEST_DIR/$name/${input_name}.stdout"
else
    fout_file="$TEST_DIR/${name}.fout"
    stdout_file="$TEST_DIR/${name}.stdout"
fi

if ! [[ -e "$fout_file" ]]; then
    unset fout_file
fi
if ! [[ -e "$stdout_file" ]]; then
    unset stdout_file
fi

# Run and pipe to test.stdout
SECONDS=0
"${cmd[@]}" > test.stdout 2> test.stderr
code=$?
execution_time=$SECONDS

exitcode=0;

function print_stderr() {
    if [[ -s test.stderr ]]; then
        if [[ $1 == "true" ]]; then
            warn "Stderr is not empty:"
        fi
        cat test.stderr
        echo
    fi
}

if [[ $code == 0 ]]; then
    # Check stdout
    if [[ -v stdout_file ]] && ! diff -q "$stdout_file" test.stdout > /dev/null
    then
        fail The standard outputs differ:
        show_diff "$stdout_file" test.stdout
        print_stderr false
        exit 2
    fi

    # Check file output
    if [[ -v fout_file ]] && ! diff -q "$fout_file" test.fout > /dev/null
    then
        fail The file outputs differ:
        show_diff "$fout_file" test.fout
        print_stderr false
        exit 2
    fi

    print_stderr true

    # Check valgrind report
    if [[ -s test.valgrind ]]; then
        warn "Valgrind reported errors in $name:"
        cat test.valgrind
        echo
        exit 3
    fi

    pass ${execution_time}s
    exit 0
else
    fail exited with code $code:
    print_stderr false
    exit 2
fi
