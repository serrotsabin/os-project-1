#!/bin/bash
# Collect system information for CIS Phase 1

echo "Collecting system information..."

mkdir -p appendix

cat > appendix/system_info.txt << 'SYSINFO'
System Information for CIS Phase 1 Experiments
===============================================

Target Deployment Platform: home.cs.siue.edu

SYSINFO

echo "Operating System:" >> appendix/system_info.txt
uname -a >> appendix/system_info.txt

echo "" >> appendix/system_info.txt
echo "Linux Distribution:" >> appendix/system_info.txt
cat /etc/os-release | grep PRETTY_NAME >> appendix/system_info.txt

echo "" >> appendix/system_info.txt
echo "Shell:" >> appendix/system_info.txt
bash --version | head -1 >> appendix/system_info.txt

echo "" >> appendix/system_info.txt
echo "Compiler:" >> appendix/system_info.txt
gcc --version | head -1 >> appendix/system_info.txt

echo "" >> appendix/system_info.txt
echo "Hardware:" >> appendix/system_info.txt
cat /proc/cpuinfo | grep "model name" | head -1 >> appendix/system_info.txt
echo -n "Memory: " >> appendix/system_info.txt
free -h | grep Mem >> appendix/system_info.txt

echo "" >> appendix/system_info.txt
echo "Experiment Date:" >> appendix/system_info.txt
date >> appendix/system_info.txt

echo "✓ System info saved to appendix/system_info.txt"
cat appendix/system_info.txt