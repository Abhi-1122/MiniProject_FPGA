#ifndef STUDENT_GEMM_INT8_H
#define STUDENT_GEMM_INT8_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Group-quantized int8 GEMM: C[M x N] = dequant(Aq, As) * dequant(Bq, Bs).
 *
 * Layout convention used by this local scaffold (see README.md — unverified
 * against the official kit, but self-consistent and testable):
 *
 *   Aq: M x K int8, row-major.        As: M x (K/GS) float32 scales.
 *   Bq: N x K int8, row-major (B stored transposed, one row per output col).
 *   Bs: N x (K/GS) float32 scales.
 *
 *   C[m][n] = sum over groups g of
 *               As[m][g] * Bs[n][g] * (sum over k in group g of Aq[m][k] * Bq[n][k])
 *
 * K must be divisible by GS.
 */
void student_gemm_int8(const int8_t *Aq, const float *As,
                        const int8_t *Bq, const float *Bs,
                        float *C, int M, int K, int N, int GS);

#ifdef __cplusplus
}
#endif

#endif /* STUDENT_GEMM_INT8_H */
