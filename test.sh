#!/bin/bash

# Directories
TEST_DIR="test"
COMPILER="./lisp-compiler"
COMMON_LISP_COMPILER="sbcl --script"

# Counters
total_tests=0
passed_tests=0

# Run tests in a folder
run_tests() {
    local folder="$1"
    local test_type="$2"

    for file in "$folder"/*.lisp "$folder"/*.lsp; do
        [[ -f "$file" ]] || continue
        ((total_tests++))

        output_file="$file.out"
        rm -f "$output_file"

        case "$test_type" in
            "compile")
                "$COMPILER" "$file" &>/dev/null
                if [[ $? -eq 0 ]]; then
                    echo "[PASS] Successfully compiled: $file"
                    ((passed_tests++))
                else
                    echo "[FAIL] Compilation failed: $file"
                fi
                ;;

            "compile-error")
                "$COMPILER" "$file" &>/dev/null
                if [[ $? -ne 0 ]]; then
                    echo "[PASS] Correctly failed to compile: $file"
                    ((passed_tests++))
                else
                    echo "[FAIL] Compilation unexpectedly succeeded: $file"
                fi
                ;;

            "exec")
                "$COMPILER" "$file" &>/dev/null
                if [[ $? -eq 0 ]]; then
                    # Run the output file
                    "$output_file" >"$output_file.actual" 2>/dev/null
                    # Run the common lisp compiler for expected output
                    $COMMON_LISP_COMPILER "$file" >"$output_file.expected" 2>/dev/null

                    # Compare actual and expected outputs
                    if diff -wB "$output_file.expected" "$output_file.actual" &>/dev/null; then
                        echo "[PASS] Output matches: $file"
                        ((passed_tests++))
                    else
                        echo "[FAIL] Output mismatch: $file"
                        echo "[INFO] Expected:\n$(cat "$output_file.expected")"
                        echo "[INFO] Got:\n$(cat "$output_file.actual")"
                    fi

                    # Clean up
                    rm -f "$output_file.expected" "$output_file.actual"
                else
                    echo "[FAIL] Execution failed: $file"
                fi
                ;;

            "runtime-error")
                "$COMPILER" "$file" &>"$output_file"
                if [[ $? -eq 0 ]]; then
                    # Run the output file and check for runtime errors
                    "$output_file" &>/dev/null
                    if [[ $? -ne 0 ]]; then
                        echo "[PASS] Correctly failed at runtime: $file"
                        ((passed_tests++))
                    else
                        echo "[FAIL] Expected runtime error but executed successfully: $file"
                    fi
                else
                    echo "[FAIL] Compilation failed: $file"
                fi
                ;;
        esac
    done
}

# Run all test cases
run_tests "$TEST_DIR/compile" "compile"
run_tests "$TEST_DIR/compile-fail" "compile-error"
run_tests "$TEST_DIR/exec" "exec"
run_tests "$TEST_DIR/exec-fail" "runtime-error"

# Summary
echo "Total tests: $total_tests"
echo "Passed tests: $passed_tests"
echo "Success rate: $((100 * passed_tests / total_tests))%"

