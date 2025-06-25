# CppLiquid - 3D Liquid Simulation

A C++23 OpenGL application that simulates multi-colored blob-based liquids in a 3D environment with a top-down view.

## Features
- Blob-based liquid simulation with multiple colored particle groups
- 3D walls bounding the simulation area with aesthetic lighting
- Top-down camera view
- Real-time physics simulation with pressure and viscosity forces
- Particle-particle and particle-wall collision detection
- 60Hz V-Sync enabled animation
- GPU-accelerated rendering with OpenGL 4.5

## Dependencies
- C++23 compiler
- CMake 3.25+
- OpenGL 4.5+
- GLEW
- GLFW3
- GLM
- Boost

## Building
```bash
./b
```

Or manually:
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

## Running
```bash
./r [options]
```

Or directly:
```bash
./build/CppLiquid [options]
```

### Command Line Options
- `--width <width>` - Set window width (default: 1280)
- `--height <height>` - Set window height (default: 720)
- `--help` - Show help message

### Examples
```bash
# Run with default settings
./r

# Run with custom window size
./r --width 1920 --height 1080

# Run fullscreen-sized
./r --width 2560 --height 1440
```

The simulation will open in a window showing colored liquid blobs bounded by 3D walls from a top-down perspective. The walls feature aesthetically pleasing off-angle lighting for better visual depth.