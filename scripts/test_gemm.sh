#!/usr/bin/env bash
# Build and run the float32 GEMM correctness + GFLOP/s harness.
#
# GEMM_SRC=path/to/file.c ./scripts/test_gemm.sh   # test a specific impl
# ./scripts/test_gemm.sh                             # test the reference impl
set -euo pipefail
cd "$(dirname "$0")/.."

[ -f build/cases_f32.txt ] || ./scripts/synth_cases.sh

GEMM_SRC="${GEMM_SRC:-src/student_gemm_ref.c}"

gcc -O2 -std=c11 -D_POSIX_C_SOURCE=200112L -Wall -Isrc -o build/test_gemm \
    test/test_gemm_driver.c "$GEMM_SRC" -lm

./build/test_gemm build/cases_f32.txt
