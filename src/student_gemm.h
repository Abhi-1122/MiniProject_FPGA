#ifndef STUDENT_GEMM_H
#define STUDENT_GEMM_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * C[M x N] = A[M x K] * B[K x N], all row-major, float32.
 *
 * M: output rows (A rows, C rows)
 * K: reduction dimension (A columns, B rows)
 * N: output columns (B columns, C columns; vector count)
 */
void student_gemm(const float *A, const float *B, float *C, int M, int K, int N);

#ifdef __cplusplus
}
#endif

#endif /* STUDENT_GEMM_H */
