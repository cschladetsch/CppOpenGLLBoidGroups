# CppLiquid - 3D Liquid Simulation

A C++23 OpenGL application that simulates multi-colored blob-based liquids in a 3D environment with a top-down view.

## Features
- Blob-based liquid simulation with multiple colored particle groups
- 3D walls bounding the simulation area
- Top-down camera view
- Real-time physics simulation with pressure and viscosity forces
- Particle-particle and particle-wall collision detection

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
mkdir build
cd build
cmake ..
make
```

## Running
```bash
./CppLiquid
```

The simulation will open in a window showing colored liquid blobs bounded by 3D walls from a top-down perspective.