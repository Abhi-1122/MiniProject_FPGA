/* Correctness + GOP/s harness for student_gemm_int8().
 * Usage: test_gemm_int8 <cases_file> <N> <GS>
 * cases_file lines: "<name> <M> <K>"
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "student_gemm_int8.h"

static void trusted_reference_gemm_int8(const int8_t *Aq, const float *As,
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

static double now_seconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

/* Fill `rows` x `K` with random floats, then quantize per (row, group of GS
 * along K) into int8 + per-group float32 scale. */
static void gen_quantized(int rows, int K, int GS, unsigned int *seed,
                           int8_t **q_out, float **s_out)
{
    int groups = K / GS;
    float *f = malloc((size_t)rows * K * sizeof(float));
    int8_t *q = malloc((size_t)rows * K * sizeof(int8_t));
    float *s = malloc((size_t)rows * groups * sizeof(float));

    for (int i = 0; i < rows * K; i++) {
        f[i] = (((float)rand_r(seed) / (float)RAND_MAX) * 2.0f - 1.0f);
    }

    for (int r = 0; r < rows; r++) {
        for (int g = 0; g < groups; g++) {
            int base = g * GS;
            float maxabs = 1e-9f;
            for (int k = 0; k < GS; k++) {
                float v = fabsf(f[r * K + base + k]);
                if (v > maxabs) maxabs = v;
            }
            float scale = maxabs / 127.0f;
            s[r * groups + g] = scale;
            for (int k = 0; k < GS; k++) {
                float v = f[r * K + base + k] / scale;
                int iv = (int)lrintf(v);
                if (iv > 127) iv = 127;
                if (iv < -127) iv = -127;
                q[r * K + base + k] = (int8_t)iv;
            }
        }
    }

    free(f);
    *q_out = q;
    *s_out = s;
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "usage: %s <cases_file> <N> <GS>\n", argv[0]);
        return 2;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "cannot open cases file: %s\n", argv[1]);
        return 2;
    }
    int N = atoi(argv[2]);
    int GS = atoi(argv[3]);

    char name[128];
    int M, K;
    int all_pass = 1;

    while (fscanf(f, "%127s %d %d", name, &M, &K) == 3) {
        if (K % GS != 0) {
            printf("%-16s SKIP (K=%d not divisible by GS=%d)\n", name, K, GS);
            continue;
        }

        unsigned int seedA = 1111u, seedB = 2222u;
        int8_t *Aq, *Bq;
        float *As, *Bs;
        gen_quantized(M, K, GS, &seedA, &Aq, &As);
        gen_quantized(N, K, GS, &seedB, &Bq, &Bs);

        float *Cref = malloc((size_t)M * N * sizeof(float));
        float *Ctest = malloc((size_t)M * N * sizeof(float));

        trusted_reference_gemm_int8(Aq, As, Bq, Bs, Cref, M, K, N, GS);

        int iters = (M * K * N > 4000000) ? 1 : 5;
        double best = 1e30;
        for (int it = 0; it < iters; it++) {
            double t0 = now_seconds();
            student_gemm_int8(Aq, As, Bq, Bs, Ctest, M, K, N, GS);
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

        double ops = 2.0 * (double)M * (double)K * (double)N;
        double gops = best > 0 ? ops / best / 1e9 : 0.0;

        printf("%-16s M=%-6d K=%-6d N=%-4d GS=%-3d  %-4s  max_err_ratio=%.3f  %.3f GOP/s\n",
               name, M, K, N, GS, pass ? "PASS" : "FAIL", max_err, gops);

        free(Aq); free(As); free(Bq); free(Bs); free(Cref); free(Ctest);
    }

    fclose(f);

    if (!all_pass) {
        printf("\nRESULT: FAIL\n");
        return 1;
    }
    printf("\nRESULT: PASS\n");
    return 0;
}
