#include "student_gemm_int8.h"

/* Naive reference implementation — correct, not fast. */
void student_gemm_int8(const int8_t *Aq, const float *As,
                        const int8_t *Bq, const float *Bs,
                        float *C, int M, int K, int N, int GS)
{
    int groups = K / GS;

    for (int m = 0; m < M; m++) {
        for (int n = 0; n < N; n++) {
            float acc = 0.0f;
            for (int g = 0; g < groups; g++) {
                int32_t isum = 0;
                int base = g * GS;
                for (int k = 0; k < GS; k++) {
                    isum += (int32_t)Aq[m * K + base + k] * (int32_t)Bq[n * K + base + k];
                }
                acc += As[m * groups + g] * Bs[n * groups + g] * (float)isum;
            }
            C[m * N + n] = acc;
        }
    }
}
