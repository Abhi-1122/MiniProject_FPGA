#!/usr/bin/env bash
# GEMM.sbatch — run the full HLS front-end (csim -> csynth -> cosim) as a
# batch job on the RCE Slurm cluster.  Submit from this directory:
#
#     sbatch GEMM.sbatch
#     squeue -u $USER           # watch it
#     tail -f gemm_<jobid>.log  # follow the output
#
# TODO: needs src/gemm_kernel.cpp + test/gemm_kernel_tb.cpp (see hls.cfg) —
# this will fail until those exist. Part 1 itself doesn't need HLS; this is
# only for when the GEMM kernel gets synthesized to hardware.

#SBATCH --job-name=gemm-hls
#SBATCH --partition=debug
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8          # within the 8-vcore student cap
#SBATCH --mem=16G                  # within the 32 GB cap
#SBATCH --time=00:30:00
#SBATCH --output=gemm_%j.log

set -eo pipefail
cd "$SLURM_SUBMIT_DIR"
source "$SLURM_SUBMIT_DIR/env.sh" >/dev/null

echo "### Host: $(hostname)   Job: $SLURM_JOB_ID"
echo "### [1/3] CSIM  (functional C simulation)"
vitis-run --mode hls --csim  --config hls.cfg --work_dir hls_work
echo "### [2/3] CSYNTH (HLS synthesis -> RTL + timing/area)"
v++ -c       --mode hls        --config hls.cfg --work_dir hls_work
echo "### [3/3] COSIM (RTL co-simulation vs testbench)"
vitis-run --mode hls --cosim --config hls.cfg --work_dir hls_work
echo "### DONE — synthesis report: hls_work/hls/syn/report/gemm_kernel_csynth.rpt"
