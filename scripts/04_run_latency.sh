#!/bin/bash
# Launch latency experiment (requires manual input)

echo "=========================================="
echo "Broadcast Latency Experiment"
echo "=========================================="
echo ""
echo "This experiment requires TWO terminals:"
echo ""
echo "Terminal 1 (THIS ONE):"
echo "  - Runs the server"
echo "  - You type commands here"
echo ""
echo "Terminal 2 (OPEN NOW):"
echo "  - SSH to server again"
echo "  - Run: ./client"
echo ""
echo "Instructions:"
echo "  1. Press Enter to start server"
echo "  2. Open Terminal 2 and run: ./client"
echo "  3. Come back here and type commands"
echo "  4. Type ~50 characters total (ls, pwd, etc.)"
echo "  5. Press Ctrl+Q when done"
echo ""
echo "Press Enter to start server..."
read

# Clean up old socket
rm -f /tmp/cis_test.sock

# Start server
echo ""
echo "Server starting..."
echo ""
./server

echo ""
echo "=========================================="
echo "Latency test complete!"
echo "Results saved to: appendix/broadcast_latency.txt"
echo ""

if [ -f appendix/broadcast_latency.txt ]; then
    COUNT=$(tail -n +2 appendix/broadcast_latency.txt | wc -l)
    echo "Measurements collected: $COUNT"
    echo ""
    echo "Summary (last 10 rows):"
    tail -10 appendix/broadcast_latency.txt
else
    echo "WARNING: broadcast_latency.txt not found!"
fi

echo "=========================================="