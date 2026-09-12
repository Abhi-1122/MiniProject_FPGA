#!/usr/bin/env bash
# Build and run the int8 GEMM correctness + GOP/s harness.
#
# N=32 GS=32 GEMM_INT8_SRC=path/to/file.c ./scripts/test_gemm_int8.sh
# ./scripts/test_gemm_int8.sh    # reference impl, N=1 (GEMV), GS=32
set -euo pipefail
cd "$(dirname "$0")/.."

[ -f build/cases_int8.txt ] || ./scripts/synth_cases.sh

GEMM_SRC="${GEMM_INT8_SRC:-src/student_gemm_int8_ref.c}"
N="${N:-1}"
GS="${GS:-32}"

gcc -O2 -std=c11 -D_POSIX_C_SOURCE=200112L -Wall -Isrc -o build/test_gemm_int8 \
    test/test_gemm_int8_driver.c "$GEMM_SRC" -lm

./build/test_gemm_int8 build/cases_int8.txt "$N" "$GS"
