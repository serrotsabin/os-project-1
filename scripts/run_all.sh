#!/bin/bash
# Master script - runs all experiments in sequence

echo "=========================================="
echo "CIS Phase 1 - Complete Experiment Suite"
echo "=========================================="
echo ""

# Step 1: System Info
./scripts/01_system_info.sh
echo ""

# Step 2: Compile
./scripts/02_compile.sh
echo ""

# Step 3: Disconnect Tests (Automated)
echo "Next: Disconnect Tests (automated)"
echo "Press Enter to continue..."
read
./scripts/03_disconnect_test.sh
echo ""

# Step 4: Latency (Manual)
echo "Next: Latency Test (requires manual input)"
echo "Press Enter to continue..."
read
./scripts/04_run_latency.sh
echo ""

echo "=========================================="
echo "All experiments complete!"
echo "=========================================="
echo ""
echo "Generated files:"
ls -lh appendix/
echo ""
echo "Next step:"
echo "  Review logs: cat appendix/*.txt appendix/*.log"
echo ""