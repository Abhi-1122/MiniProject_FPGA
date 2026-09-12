#include "student_gemm.h"

/* Naive reference implementation — correct, not fast. Used as the default
 * GEMM_SRC when none is given, so `./scripts/test_gemm.sh` with no env
 * var is a sanity check of the harness itself. */
void student_gemm(const float *A, const float *B, float *C, int M, int K, int N)
{
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            float acc = 0.0f;
            for (int k = 0; k < K; k++) {
                acc += A[i * K + k] * B[k * N + j];
            }
            C[i * N + j] = acc;
        }
    }
}
