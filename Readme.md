# CIS Phase 1 - Experimental Validation

## Team Members
- Abin Timalsina
- Nuraj Rimal
- Sabin Ghimire

## Overview
Validation experiments for Collaborative Interactive Shell design.

## Quick Start
```bash
# 1. Compile programs
make

# 2. Run all experiments
make experiments

# OR run individually:
./scripts/01_system_info.sh
./scripts/02_compile.sh
./scripts/03_run_latency.sh      # Manual: requires 2 terminals
./scripts/04_disconnect_test.sh
```

## Experiment 1: System Information (Automated)

Collects deployment platform specifications.
```bash
./scripts/01_system_info.sh
```

**Output:** `appendix/system_info.txt`

## Experiment 2: Broadcast Latency (Manual)

Measures latency with 1 controller + 1 observer.

### Terminal 1 (Server):
```bash
./scripts/03_run_latency.sh
# Wait for "Press Enter"
# Press Enter
# Type commands when prompted
```

### Terminal 2 (Client):
```bash
# Open NEW SSH session
cd ~/cis_phase1
./client
```

### Back to Terminal 1:
Type ~50 characters total:
```bash
ls
pwd
whoami
date
echo "test 1"
echo "test 2"
# ... continue until ~50 chars
# Press Ctrl+Q when done
```

**Output:** `appendix/broadcast_latency.txt`

## Experiment 3: Disconnect Robustness (Automated)

Tests controller disconnect scenarios.
```bash
./scripts/04_disconnect_test.sh
```

**Output:** `appendix/disconnect_test.log`

## Files
```
cis_phase1/
├── server.c              # PTY server with observer support
├── client.c              # Observer client
├── Makefile              # Build automation
├── README.md             # This file
├── scripts/
│   ├── 01_system_info.sh
│   ├── 02_compile.sh
│   ├── 03_run_latency.sh
│   ├── 04_disconnect_test.sh
│   └── run_all.sh
└── appendix/             # Generated experiment data
    ├── system_info.txt
    ├── broadcast_latency.txt
    └── disconnect_test.log
```

## Platform

- **Target:** home.cs.siue.edu
- **Tested on:** Linux (see system_info.txt)
- **Development:** macOS (prototyping)

## Notes

- Script 03 (latency) requires manual input for realistic measurements
- Scripts 01 and 04 are fully automated
- Socket path: `/tmp/cis_test.sock`
- Phase 2 will add multiple observers and floor control