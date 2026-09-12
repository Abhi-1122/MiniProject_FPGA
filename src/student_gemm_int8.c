#include "student_gemm_int8.h"

/*
 * TODO: implement. Start with the obvious per-group integer dot product
 * (accumulate int32, scale once per group), then look at batching the N>1
 * case and reusing loaded A/B groups across output columns/rows.
 */
void student_gemm_int8(const int8_t *Aq, const float *As,
                        const int8_t *Bq, const float *Bs,
                        float *C, int M, int K, int N, int GS)
{
    (void)Aq;
    (void)As;
    (void)Bq;
    (void)Bs;
    (void)K;
    (void)GS;

    for (int i = 0; i < M * N; i++) {
        C[i] = 0.0f;
    }
}
