#!/usr/bin/env bash
# Generate the fixed test-case lists used by test_gemm.sh / test_gemm_int8.sh.
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build

cat > build/cases_f32.txt <<'EOF'
attn-proj_n1    288   288   1
attn-proj_n32   288   288   32
ffn-up_n1       768   288   1
ffn-up_n32      768   288   32
ffn-down_n1     288   768   1
ffn-down_n32    288   768   32
classifier_n1   32000 288   1
classifier_n32  32000 288   32
EOF

cat > build/cases_int8.txt <<'EOF'
attn-proj    288   288
ffn-up       768   288
ffn-down     288   768
classifier   32000 288
EOF

echo "wrote build/cases_f32.txt and build/cases_int8.txt"
