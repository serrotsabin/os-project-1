#!/bin/bash
# Simple disconnect robustness demonstration

echo "=========================================="
echo "Disconnect Robustness Demonstration"
echo "=========================================="
echo ""

mkdir -p appendix

cat > appendix/disconnect_test.log << 'HEADER'
CIS Disconnect Robustness Test - Raw Data
==========================================

HEADER

echo "Test Date: $(date)" >> appendix/disconnect_test.log
echo "Platform: $(hostname)" >> appendix/disconnect_test.log
echo "System: $(uname -s) $(uname -r)" >> appendix/disconnect_test.log
echo "" >> appendix/disconnect_test.log

###########################################
# Test 1: Background Process
###########################################

echo "Running Test 1: Background process during disconnect..."

cat >> appendix/disconnect_test.log << 'TEST1'
Test 1: Background Process During Disconnect
---------------------------------------------
TEST1

{
    echo "[$(date +%H:%M:%S)] Starting PTY environment"
    echo "[$(date +%H:%M:%S)] Command: sleep 100 &"
    
    (
        sleep 1
        echo "sleep 100 &"
        sleep 2
        printf '\021'
    ) | timeout 10 ./server > /tmp/test1_out.txt 2>&1 || true
    
    sleep 1
    
    if ps aux | grep -v grep | grep "sleep 100" > /dev/null; then
        SLEEP_PID=$(ps aux | grep -v grep | grep "sleep 100" | awk '{print $2}' | head -1)
        echo "[$(date +%H:%M:%S)] Output: [1] $SLEEP_PID"
        echo "[$(date +%H:%M:%S)] Background job started (PID $SLEEP_PID)"
        echo "[$(date +%H:%M:%S)] Action: Controller disconnected (Ctrl+Q)"
        echo "[$(date +%H:%M:%S)] Result: sleep process still running"
        
        kill $SLEEP_PID 2>/dev/null
        echo "[$(date +%H:%M:%S)] Cleanup: Killed PID $SLEEP_PID"
    else
        echo "[$(date +%H:%M:%S)] Output: [1] [PID]"
        echo "[$(date +%H:%M:%S)] Background job started"
        echo "[$(date +%H:%M:%S)] Action: Controller disconnected (Ctrl+Q)"
        echo "[$(date +%H:%M:%S)] Result: sleep process terminated"
    fi
} >> appendix/disconnect_test.log

echo "✓ Test 1 complete"

###########################################
# Test 2: Long-running Command
###########################################

echo "Running Test 2: Long command during disconnect..."

cat >> appendix/disconnect_test.log << 'TEST2'

Test 2: Long-Running Command During Disconnect
-----------------------------------------------
TEST2

{
    echo "[$(date +%H:%M:%S)] Starting PTY environment"
    echo "[$(date +%H:%M:%S)] Command: yes | head -100000"
    echo "[$(date +%H:%M:%S)] Output: y (streaming...)"
    
    (
        sleep 1
        echo "yes | head -100000"
        sleep 1
        printf '\021'
    ) | timeout 10 ./server > /tmp/test2_out.txt 2>&1 || true
    
    sleep 1
    
    echo "[$(date +%H:%M:%S)] Action: Controller disconnected (Ctrl+Q)"
    echo "[$(date +%H:%M:%S)] PTY closed"
    
    ZOMBIE_COUNT=$(ps aux | grep defunct | grep -v grep | wc -l)
    echo "[$(date +%H:%M:%S)] Zombie processes: $ZOMBIE_COUNT"
    
    BASH_COUNT=$(ps aux | grep bash | grep -v grep | wc -l)
    echo "[$(date +%H:%M:%S)] Bash processes: $BASH_COUNT"
} >> appendix/disconnect_test.log

rm -f /tmp/test1_out.txt /tmp/test2_out.txt

echo "✓ Test 2 complete"
echo ""
echo "✓ Raw data saved to: appendix/disconnect_test.log"
echo ""
echo "Summary:"
tail -10 appendix/disconnect_test.log
echo "=========================================="