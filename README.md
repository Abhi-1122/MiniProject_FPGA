# labs/tinystories — Part 1 (GEMM) local scaffold

**This is a stand-in, not the official kit.** The real starter code for this
project is distributed through GitHub Classroom (link on Moodle) and will
include the actual `synth_cases.sh` / `test_gemm.sh` / reference source and
the exact fixed test cases that get graded. This directory reproduces the
*documented* contract from the course site closely enough to start writing
and iterating on `student_gemm` today. When the official repo lands:

1. Clone it alongside this one (or into this directory).
2. Diff its `src/student_gemm.h` / `student_gemm_int8.h` against ours — the
   float32 signature is given verbatim on the course site so it should match
   exactly. The int8 layout convention below (see `src/student_gemm_int8.h`)
   is a reasonable guess, not confirmed — expect to adjust it.
3. Drop your `student_gemm.c` / `student_gemm_int8.c` bodies into the real
   `src/` — the function bodies are what you're building, not this harness.

## Layout

```
src/
  student_gemm.h            prototype (float32) — matches course spec
  student_gemm.c             <-- YOU write this. Currently a TODO stub.
  student_gemm_ref.c         naive reference impl (used when GEMM_SRC unset)
  student_gemm_int8.h        prototype (int8, group-quantized)
  student_gemm_int8.c        <-- YOU write this. Currently a TODO stub.
  student_gemm_int8_ref.c    naive reference impl (used when GEMM_SRC unset)
test/
  test_gemm_driver.c         float32 correctness + GFLOP/s harness
  test_gemm_int8_driver.c    int8 correctness + GOP/s harness
scripts/
  synth_cases.sh              generates build/cases_f32.txt, build/cases_int8.txt
  test_gemm.sh                 build + run the float32 harness
  test_gemm_int8.sh            build + run the int8 harness
build/                        generated, git-ignored
```

## Workflow

```bash
./scripts/synth_cases.sh                       # generate test cases (run once)
./scripts/test_gemm.sh                          # test the reference impl (sanity check)
GEMM_SRC=src/student_gemm.c ./scripts/test_gemm.sh     # test your impl

./scripts/test_gemm_int8.sh                      # int8 reference, N=1 (GEMV)
N=32 ./scripts/test_gemm_int8.sh                  # int8 reference, N=32 (batched)
GEMM_INT8_SRC=src/student_gemm_int8.c N=32 ./scripts/test_gemm_int8.sh   # your impl
```

Correctness tolerance: 1e-2 (relative+absolute), matching the course spec.

## Shapes under test (from the course site)

| Name | M | K | Notes |
|---|---|---|---|
| attn-proj | 288 | 288 | square |
| ffn-up | 768 | 288 | tall |
| ffn-down | 288 | 768 | wide |
| classifier | 32000 | 288 | memory-bound |

Each is tested at N=1 (GEMV, memory-bound) and N=32 (GEMM, compute-bound,
tileable) for float32. int8 defaults to N=1 unless overridden.

## int8 layout convention (placeholder — verify against the real kit)

`student_gemm_int8(Aq, As, Bq, Bs, C, M, K, N, GS)`:

- `Aq` is `M x K` int8, row-major, quantized per (row, group-of-GS-along-K).
- `As` is `M x (K/GS)` float32 scales, one per (row, group).
- `Bq` is `N x K` int8, row-major — **B stored transposed** (one row per
  output column), quantized per (column, group-of-GS-along-K), mirroring A.
- `Bs` is `N x (K/GS)` float32 scales.
- `C[m][n] = sum over groups g of As[m][g] * Bs[n][g] * (sum over k in g of
  Aq[m][k] * Bq[n][k])`.

`K` must be divisible by `GS`. This convention is chosen to be self-consistent
and testable, not copied from an unseen official spec — treat it as your
working hypothesis until you can confirm it against the real repo.
