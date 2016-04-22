#!/usr/bin/env bash
# Author: fridex
# Changes:

# Directory with the test tree.
TEST_DIR='./TEST'
# Executable names.
IFJ11_EXEC="ifj11"
IFJ11_DEBUG="ifj11-debug"
DEBUG_OUT="./run.log"
# An executable to be run.
EXECUTABLE=""

################################################################################

# Function prints test relative path.
# @param - name of the test
function make_test_dir() {
    if grep -q '^TEST/' <<< "$1"; then
        echo "$1";
    else
        echo "TEST/$1"
    fi
}

# Check if test files exist
# @param - test name
function check_test_result() {
    if [ -z "$1" ]; then
        echo "> ERROR: check test: No test specified." 1>&2
        exit 1
    fi

    PASSED=0 # "0" - not run
             # "1" - passed
             # "2" - not passed

    FILE=`make_test_dir "$1"`
    # Check return value.
    if [ -f "$FILE/return" ]; then
        if [ `cat "$FILE/T_return"` \
            -eq `cat "$FILE/return"` ]; then
            echo "= return value:		OK"
            PASSED=1
        else
            echo "= return value:		FAILED"
            PASSED=2
        fi
    else
        echo "= return value:		UNKNOWN"
        if [ "$PASSED" -eq 1 ]; then
            PASSED=2
        fi
    fi

    # Check stderr.
    if [ -f "$FILE/stderr" ]; then
        # Check if T_stderr exist too.
        # if T_stderr is nonzero in size, stderr should be too
        # if T_stderr is zero, stderr should be zero-sized too
        if [ ! -s "$FILE/T_stderr" -a ! -s "$FILE/stderr" ] || \
            [  -s "$FILE/T_stderr" -a -s "$FILE/stderr" ]; then
            echo "= stderr:		OK"
        else
            echo "= stderr:		FAILED"
            PASSED=2
        fi
    else
        echo "= stderr:		UNKNOWN"
        if [ "$PASSED" -eq 1 ]; then
            PASSED=2
        fi
    fi

    # Check stdout.
    if [ -f "$FILE/stdout" ]; then
        if diff -q "$FILE/T_stdout"       \
            "$FILE/stdout" >/dev/null; then
            echo "= stdout:		OK"
        else
            echo "= stdout:		FAILED"
            PASSED=2
        fi
    else
        echo "= stdout:		UNKNOWN"
        if [ "$PASSED" -eq 1 ]; then
            PASSED=2
        fi
    fi

    # Check valgrind output
    if [ -f "$FILE/valgrind" ]; then
        if grep -q "ERROR SUMMARY: 0 errors"     \
            "$FILE/valgrind"; then
            echo "= valgrind:		OK"
        else
            echo "= valgrind:		FAILED"
        fi
    else
            echo "= valgrind:		DISABLED"
    fi

    # PRINT RESULT!
    echo "================================"
    if [ "$PASSED" -eq 0 ]; then
        echo " = $FILE:		NOT RUN ="
    elif [ "$PASSED" -eq 1 ]; then
        echo "= $FILE:		PASSED ="
    else
        echo "= $FILE:		FAILED ="
    fi
}

# Print a help message.
# @param - none
function print_help() {
    echo "Testing script for IFJ11."
    echo "Usage: $0 [OPTIONS]... [TESTS]..."
    echo ""
    echo "Options:"
    echo "-h|--help       - print this simple help"
    echo "--test NUM1 NUM2"
    echo "-t NUM1 NUM2    - run test number NUM1 and NUM2"
    echo "--lua NUM1 NUM2"
    echo "-l NUM1 NUM2    - run lua intepreter on tests"
    echo "--run NUM1 NUM2"
    echo "-r NUM1 NUM2    - run ifj11 interpreter on test interactively"
    echo "-a|--all        - run all tests"
    echo "-i|--info NUM   - print info of the test NUM"
    echo "-c|--clean      - clean test files and $DEBUG_OUT"
}

# Run tests.
# @param - tests to be run
function run_test() {
    if [ -z "$1" ]; then
        echo "> ERROR: No test specified." 1>&2
        exit 1
    fi

    for test in $@; do
        FILE=`make_test_dir "$test"`
        echo "################################"
        echo "> Executing test $FILE."

        if [ ! -d "$FILE/" ]; then
            echo "> ERROR: Test $TEST_DIR/$test does not exist." 1>&2
            exit 1
        fi

        if [ ! -f "$FILE/T_stdin" ]; then
            echo "> ERROR: No input for test $TEST_DIR/$test." 1>&2
            exit 1
        fi

        # OK, enought tests for test. Let's test!

        # First clean up run.log file.
        if [ -f "$DEBUG_OUT" ]; then
            rm "$DEBUG_OUT"
        fi

        # Execute first test.
        PARAM=`cat "$FILE/T_param"`

        eval "$EXECUTABLE" $PARAM                                        \
                        < "$FILE/T_stdin"                                \
                        1> "$FILE/stdout"                                \
                        2> "$FILE/stderr"
        echo "$?" > "$FILE/return"

        # If there is a debug log, copy it to test directory.
        if [ "$EXECUTABLE" = "./$IFJ11_DEBUG" -a -f "$DEBUG_OUT" ]; then
            cp "$DEBUG_OUT" "$FILE/"
        fi

        # If there is a valgrind log its output.
        if which valgrind >/dev/null; then
            valgrind --leak-check=full --show-reachable=yes                  \
                --log-file="$FILE/valgrind"                                  \
                "$EXECUTABLE" "$FILE/T_program.lua"                          \
                            < "$FILE/T_stdin"                                \
                            1>/dev/null                                      \
                            2>/dev/null
        fi

        # Print test result.
        check_test_result "$test"
    done
}

# Run given file in Lua interpreter.
# @param - tests to be run
function run_lua() {
    if [ -z "$1" ]; then
        echo "> ERROR: No test specified." 1>&2
        exit 1
    fi
    for test in $@; do
        FILE=`make_test_dir "$test"`
        echo "################################"
        echo "> Executing test $FILE in Lua."

        # Check test files.
        if [ ! -d "$FILE" ]; then
            echo "> ERROR: Test $FILE does not exist." 1>&2
            exit 1
        fi

        if [ ! -f "$FILE/T_stdin" ]; then
            echo "> ERROR: No input for test $FILE." 1>&2
            exit 1
        fi

        TMPFILE=`mktemp /tmp/ifj11.XXXXXX`
        cat "$TEST_DIR/ifj11.lua" "$FILE/T_program.lua" > "$TMPFILE"

        export LUA_INIT=@"$TMPFILE"
        lua -e "main()"

        rm "$TMPFILE"
    done
}

# Run given file in ifj11 interpreter interactively.
# @param - tests to be run
function run_ifj11() {
    if [ -z "$1" ]; then
        echo "> ERROR: No test specified." 1>&2
        exit 1
    fi
    for test in $@; do
        FILE=`make_test_dir "$test"`
        echo "################################"
        echo "> Executing test $FILE in ifj11."

        # Check test files.
        if [ ! -d "$FILE" ]; then
            echo "> ERROR: Test $FILE does not exist." 1>&2
            exit 1
        fi

        if [ ! -f "$FILE/T_stdin" ]; then
            echo "> ERROR: No input for test $FILE." 1>&2
            exit 1
        fi

        exec "$EXECUTABLE" "$FILE"

    done
}

# Run all tests.
# @param - none
function run_test_all() {
    for test in `ls --quoting-style=escape $TEST_DIR`;  do
        # Test are in directories.
        if [ -d "$TEST_DIR/$test" ]; then
            run_test "$test"
        fi
    done

    echo "> All test executed."
}

# Print info of the test.
# @param - number of tests to be displayed
function print_test_info() {
    if [ -z "$1" ]; then
        echo "> ERROR: No test specified." 1>&2
        exit 1
    fi

    for test in $@; do
        FILE=`make_test_dir "$test"`
        if [ -d "$FILE" ]; then
            echo "################################"
            ## Print test info
            echo "= TEST $test:"
            if [ -s "$FILE/T_info" ]; then
                cat "$FILE/T_info"
            else
                echo "> ERROR no $FILE/T_info!"
            fi

            ## Print params
            if [ -f "$FILE/T_param" ]; then
                echo "= param: "
                cat "$FILE/T_param"
            else
                echo "> ERROR no $FILE/T_param!"
            fi

            ## Return value
            if [ -s "$FILE/T_return" ]; then
                echo -n "= return value:		"
                cat "$FILE/T_return"
            else
                echo "> ERROR no $FILE/T_return!"
            fi

            if [ -s "$FILE/return" ]; then
                echo -n "= test return value:	"
                RETURN=`cat "$FILE/return"`
                case "$RETURN" in
                    0)
                        echo "$RETURN - RET_OK"
                        ;;
                    1)
                        echo "$RETURN - RET_ERR_LEX"
                        ;;
                    2)
                        echo "$RETURN - RET_ERR_SYN"
                        ;;
                    3)
                        echo "$RETURN - RET_ERR_SEM"
                        ;;
                    4)
                        echo "$RETURN - RET_ERR_INT"
                        ;;
                    5)
                        echo "$RETURN - RET_ERR_MY"
                        ;;
                    *)
                        echo "$RETURN - ???"
                        ;;
                esac
            else
                echo "= test return value:	NOT RUN"
            fi

            ## stderr
            if [ -f "$FILE/T_stderr" \
                -a  ! -s "$FILE/T_stderr" ]; then
                echo "= stderr:		NONE"
            elif [ -s "$FILE/T_stderr" ]; then
                echo "= stderr:		EXPECTED"
            else
                echo "> ERROR: no $FILE/T_stderr!"
            fi

            if [ -f "$FILE/stderr" \
                -a ! -s "$FILE/stderr" ]; then
                echo "= test stderr:		NONE"
            elif [ -s "$FILE/stderr" ]; then
                echo "= test stderr:"
                cat "$FILE/stderr"
            else
                echo "= test stderr:		NOT RUN"
            fi

            ## stdout
            if [ -f "$FILE/T_stdout"  \
                -a ! -s "$FILE/T_stdout" ]; then
                echo "= stdout:		NONE"
            elif [ -s "$FILE/T_stdout" ]; then
                echo "= stdout:"
                cat "$FILE/T_stdout"
            else
                echo "> ERROR: no $FILE/T_stdout!"
            fi

            if [ -f "$FILE/stdout" \
                -a ! -s "$FILE/stdout" ]; then
                echo "= test stdout:		NONE"
            elif [ -s "$FILE/stdout" ]; then
                echo "= test stdout:"
                cat "$FILE/stdout"
            else
                echo "= test stdout:		NOT RUN"
            fi

            echo "= stdout diff:"
            diff -Nau "$FILE/T_stdout" "$FILE/stdout"

            ## stdin
            if [ -f "$FILE/T_stdin" ]; then
                echo "= stdin:"
                cat "$FILE/T_stdin"
            else
                echo "> ERROR: no $FILE/T_stdin!"
            fi

            if [ -f "$FILE/run.log" \
                -a -s "$FILE/run.log" ]; then
                echo "= debug test messages:"
                cat "$FILE/run.log"
            elif [ -f "$FILE/run.log" \
                -a ! -s "$FILE/run.log" ]; then
                echo "= debug test messages:	NONE"
            else
                echo "= debug test messages:	DISABLED"
            fi


            if [ -f "$FILE/valgrind" ]; then
                echo "= valgrind:"
                cat "$FILE/valgrind"
            else
                echo "= valgrind:		DISABLED"
            fi

            # Show if test was passed.
            echo "--------------------------------"
            check_test_result "$test"
        else
            echo "> ERROR: Test $test does not exist!" 1>&2
        fi

    done

}

# Clean test files in test directory tree.
# @param - none
function clean_test_files() {
    if [ ! -z "$1" ]; then
        echo "> ERROR: Bad param $1" 1>&2
        exit 1
    fi

    # Clear debug output.
    if [ -f "$DEBUG_OUT" ]; then
        rm "$DEBUG_OUT"
    fi

    for file in `ls --quoting-style=escape $TEST_DIR`;  do
        FILE=`make_test_dir "$file"`
        if [ -d "$FILE" ]; then
                rm -rf "$FILE/return" \
                    "$FILE/stderr"     \
                    "$FILE/stdout"     \
                    "$FILE/run.log"    \
                    "$FILE/valgrind"   \
                    "$FILE/"*~ 
        fi
    done
}

# Function asks for executable, if there is IFJ11_DEBUG and IFJ11_EXEC
# at the same time.
# @param - none
function ask_for_executable() {
            echo "> WARNING: $IFJ11_DEBUG and $IFJ11_EXEC detected!"
            echo "> Please choose your executable:"
            echo "> (1) $IFJ11_DEBUG"
            echo "> (2) $IFJ11_EXEC"
            echo -n "> "
            read key
            case "$key" in
                1)
                    EXECUTABLE="./$IFJ11_DEBUG"
                    echo "> Executable: $EXECUTABLE"
                    return 0
                    ;;
                2)
                    EXECUTABLE="./$IFJ11_EXEC"
                    echo "> Executable: $EXECUTABLE"
                    return 0
                    ;;
                *)
                    return 1
                    ;;
            esac
}

# Check if there is an executable to run.
# @param - none
function check_executable() {
    if [ -x "$IFJ11_DEBUG" ]; then
        if [ -x "$IFJ11_EXEC" ]; then
            while ! ask_for_executable; do
                continue
            done
        else
            EXECUTABLE="./$IFJ11_DEBUG"
            echo "> Executable: $EXECUTABLE"
        fi
    elif [ -x "$IFJ11_EXEC" ]; then
        EXECUTABLE="./$IFJ11_EXEC"
        echo "> Executable: $EXECUTABLE"
    else
        echo "> ERROR: Executable file $IFJ11_EXEC nor" \
             "$IFJ11_DEBUG does not exist." 1>&2
        exit 1
    fi
}

################################################################################

# Determinate option.
case "$1" in
    -h|--help)
        shift
        print_help
        ;;
    -t|--test)
        shift
        check_executable
        run_test $@
        ;;
    -l|--lua)
        shift
        run_lua $@
        ;;
    -r|--run)
        shift
        check_executable
        run_ifj11 $@
        ;;
    -a|--all)
        shift
        check_executable
        run_test_all $@
        ;;
    -i|--info)
        shift
        print_test_info $@
        ;;
    -c|--clean)
        shift
        clean_test_files
        ;;
    *)
        echo "> ERROR: Bad option: \'$1\'" 1>&2
        print_help
        exit 1;
        ;;
esac

# Default return value if there is no error.
exit 0;

################################################################################
