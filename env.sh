#!/usr/bin/env bash
# Source this to put the U50 / Vitis 2025.1 toolchain on your PATH.
#   source env.sh
# Works on rce and on every compute node (/home is a shared mount, so all the
# Xilinx paths below resolve everywhere).

# Vitis HLS + v++ (2025.1 is the version validated for the U50 on this cluster).
source /home/Xilinx/2025.1/Vitis/settings64.sh

# XRT — needed for host compilation and hw/hw_emu runs (harmless for csim/csynth).
source /opt/xilinx/xrt/setup.sh >/dev/null 2>&1 || true

# The U50 build platform (.xpfm) is NOT in /opt/xilinx/platforms on this cluster;
# it lives on the shared NFS copy. Search both so v++ hw/hw_emu links resolve it.
export PLATFORM_REPO_PATHS=/opt/xilinx/platforms:/home/Xilinx/opt/xilinx/platforms

# Convenience handles for the U50.
export PLATFORM=xilinx_u50_gen3x16_xdma_5_202210_1
export PART=xcu50-fsvh2104-2-e

# FLEXlm license (needed for v++ hw/hw_emu; csim/csynth/cosim work without it).
export LM_LICENSE_FILE="${LM_LICENSE_FILE:-2100@rce.iiit.ac.in}"

echo "[env] Vitis: $(command -v v++ 2>/dev/null || echo NOT-FOUND)  PLATFORM=$PLATFORM"
