cmake_minimum_required(VERSION 3.16)
project(CppLiquid VERSION 1.0 LANGUAGES CXX)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(GTest REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW3 REQUIRED glfw3)

# Find GLM (header-only library)
find_path(GLM_INCLUDE_DIR glm/glm.hpp)

# Create a shared library with all core functionality (excluding main.cpp)
add_library(CppLiquidCore STATIC
    Source/LiquidSimulation.cpp
    Source/Camera.cpp
    Source/Wall.cpp
    Source/Renderer.cpp
)

# Set include directories for the core library
target_include_directories(CppLiquidCore PUBLIC
    Include
    ${GLM_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

# Link libraries to core
target_link_libraries(CppLiquidCore PUBLIC
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${Boost_LIBRARIES}
    ${GLFW3_LIBRARIES}
)

# Compiler flags
target_compile_options(CppLiquidCore PRIVATE
    -Wall -Wextra -Wpedantic -O2
    ${GLFW3_CFLAGS_OTHER}
)

# Main executable
add_executable(CppLiquid Source/main.cpp)
target_link_libraries(CppLiquid PRIVATE CppLiquidCore)

# Test executable - FIXED: Now properly links with CppLiquidCore
add_executable(CppLiquidTests
    Test/TestMain.cpp
    Test/TestLiquidSimulation.cpp
    Test/TestCamera.cpp
    Test/TestWall.cpp
)

# CRITICAL FIX: Link test executable with the core library
target_link_libraries(CppLiquidTests PRIVATE
    CppLiquidCore  # This was missing!
    GTest::gtest
    GTest::gtest_main
)

# Set include directories for tests
target_include_directories(CppLiquidTests PRIVATE
    Include
    ${GLM_INCLUDE_DIR}
)

# Enable testing
enable_testing()
add_test(NAME CppLiquidTests COMMAND CppLiquidTests)

# Copy shaders to build directory
add_custom_command(TARGET CppLiquid POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Shaders $<TARGET_FILE_DIR:CppLiquid>/Shaders
)
