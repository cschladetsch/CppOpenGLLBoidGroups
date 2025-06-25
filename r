#!/bin/bash

# First build the application
./b

# Check if build succeeded
if [ $? -eq 0 ]; then
    # Run the application with all passed arguments
    ./build/CppLiquid "$@"
else
    echo "Build failed, cannot run application"
    exit 1
fi