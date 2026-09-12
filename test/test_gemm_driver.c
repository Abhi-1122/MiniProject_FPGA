/* Correctness + GFLOP/s harness for student_gemm().
 * Usage: test_gemm <cases_file>
 * cases_file lines: "<name> <M> <K> <N>"
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "student_gemm.h"

static void trusted_reference_gemm(const float *A, const float *B, float *C,
                                    int M, int K, int N)
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

static float randf(unsigned int *seed)
{
    return ((float)rand_r(seed) / (float)RAND_MAX) * 2.0f - 1.0f;
}

static double now_seconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <cases_file>\n", argv[0]);
        return 2;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "cannot open cases file: %s\n", argv[1]);
        return 2;
    }

    char name[128];
    int M, K, N;
    int all_pass = 1;

    while (fscanf(f, "%127s %d %d %d", name, &M, &K, &N) == 4) {
        unsigned int seed = 1234567u;
        float *A = malloc((size_t)M * K * sizeof(float));
        float *B = malloc((size_t)K * N * sizeof(float));
        float *Cref = malloc((size_t)M * N * sizeof(float));
        float *Ctest = malloc((size_t)M * N * sizeof(float));

        for (int i = 0; i < M * K; i++) A[i] = randf(&seed);
        for (int i = 0; i < K * N; i++) B[i] = randf(&seed);

        trusted_reference_gemm(A, B, Cref, M, K, N);

        int iters = (M * K * N > 4000000) ? 1 : 5;
        double best = 1e30;
        for (int it = 0; it < iters; it++) {
            double t0 = now_seconds();
            student_gemm(A, B, Ctest, M, K, N);
            double dt = now_seconds() - t0;
            if (dt < best) best = dt;
        }

        double max_err = 0.0;
        for (int i = 0; i < M * N; i++) {
            double diff = fabs((double)Ctest[i] - (double)Cref[i]);
            double tol = 1e-2 * (1.0 + fabs((double)Cref[i]));
            double rel = diff / tol;
            if (rel > max_err) max_err = rel;
        }
        int pass = max_err <= 1.0;
        all_pass &= pass;

        double flops = 2.0 * (double)M * (double)K * (double)N;
        double gflops = best > 0 ? flops / best / 1e9 : 0.0;

        printf("%-16s M=%-6d K=%-6d N=%-4d  %-4s  max_err_ratio=%.3f  %.3f GFLOP/s\n",
               name, M, K, N, pass ? "PASS" : "FAIL", max_err, gflops);

        free(A); free(B); free(Cref); free(Ctest);
    }

    fclose(f);

    if (!all_pass) {
        printf("\nRESULT: FAIL\n");
        return 1;
    }
    printf("\nRESULT: PASS\n");
    return 0;
}
