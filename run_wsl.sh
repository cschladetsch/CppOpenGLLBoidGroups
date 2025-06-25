#!/bin/bash

# Set display for WSL
export DISPLAY=$(grep -m 1 nameserver /etc/resolv.conf | awk '{print $2}'):0.0

# Optional: disable access control for X server (for testing)
# You may need to run "xhost +" on Windows X server

echo "Running CppLiquid with DISPLAY=$DISPLAY"
echo "Make sure your X server (VcXsrv/Xming) is running on Windows"
echo "----------------------------------------"

./build/CppLiquid "$@"