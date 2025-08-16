#!/bin/bash
# Build script for Acoustics Final project
# This handles the space in the directory name

# Create a temporary symlink without spaces
cd /Users/drewatz/Code
ln -sf "Acoustics Final" acoustics_final_build 2>/dev/null

# Build from the symlink
cd acoustics_final_build
make clean
make -j4

# Copy the ROM back to the original directory
if [ -f psychoacoustic_demo.gba ]; then
    cp psychoacoustic_demo.gba "/Users/drewatz/Code/Acoustics Final/"
    echo "Build successful! ROM created: psychoacoustic_demo.gba"
else
    echo "Build failed. Check errors above."
fi

# Clean up symlink
cd /Users/drewatz/Code
rm -f acoustics_final_build
