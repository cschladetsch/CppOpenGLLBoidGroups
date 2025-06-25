cmake_minimum_required(VERSION 3.25)
project(CppLiquid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

# Create executable
add_executable(CppLiquid
    Src/main.cpp
    Src/Renderer.cpp
    Src/LiquidSimulation.cpp
    Src/Camera.cpp
    Src/Wall.cpp
)

# Include directories
target_include_directories(CppLiquid PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

# Link libraries
target_link_libraries(CppLiquid PRIVATE
    OpenGL::GL
    GLEW::GLEW
    glfw
    glm::glm
    Boost::boost
    Boost::system
)

# Copy shaders to build directory
add_custom_command(TARGET CppLiquid POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Shaders $<TARGET_FILE_DIR:CppLiquid>/Shaders
)

# Enable testing
enable_testing()
add_subdirectory(Test)