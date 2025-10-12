#!/usr/bin/env bash
set -euo pipefail

# Compiler flags
CFLAGS="-DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined"
INCLUDE="-Igc"
OUT_DIR="out"

# Create output directory if it doesn't exist
mkdir -p "$OUT_DIR"

# Collect all test source files in tests/ (shallow)
ALL_TESTS=(tests/*.c)

# Determine which tests to run
if [ $# -eq 0 ]; then
    TESTS=("${ALL_TESTS[@]}")
else
    TESTS=()
    for arg in "$@"; do
        matched=0
        for t in "${ALL_TESTS[@]}"; do
            if [[ $(basename "$t" .c) == "$arg" ]]; then
                TESTS+=("$t")
                matched=1
                break
            fi
        done
        if [ $matched -eq 0 ]; then
            echo -e "\033[31m[ERROR]\033[0m Unknown test: $arg"
            exit 1
        fi
    done
fi

echo "=== ft-gc test runner ==="

for test_src in "${TESTS[@]}"; do
    test_name=$(basename "$test_src" .c)
    echo -e "\n\033[36m[RUNNING]\033[0m $test_name"

    exe_path="$OUT_DIR/$test_name"
    gcc $CFLAGS gc/*.c "$test_src" $INCLUDE -o "$exe_path"

    "$exe_path"

    echo -e "\033[32m[PASS]\033[0m $test_name completed successfully"
done

echo -e "\n\033[32m[ALL TESTS PASSED]\033[0m"
