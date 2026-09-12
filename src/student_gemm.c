#include "student_gemm.h"

/*
 * TODO: implement. Start with the obvious triple loop to get correctness,
 * then work on tiling / blocking / loop order / reuse to push GFLOP/s up,
 * especially for N > 1 where B tiles can be reused across output rows.
 */
void student_gemm(const float *A, const float *B, float *C, int M, int K, int N)
{
    (void)A;
    (void)B;
    (void)K;

    for (int i = 0; i < M * N; i++) {
        C[i] = 0.0f;
    }
}
