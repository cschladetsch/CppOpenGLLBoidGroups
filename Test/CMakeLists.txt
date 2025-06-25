cmake_minimum_required(VERSION 3.25)

# Find Google Test
find_package(GTest REQUIRED)

# Test executable
add_executable(CppLiquidTests
    TestMain.cpp
    TestLiquidSimulation.cpp
    TestCamera.cpp
    TestWall.cpp
)

# Include directories
target_include_directories(CppLiquidTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Include
)

# Link libraries
target_link_libraries(CppLiquidTests PRIVATE
    GTest::gtest
    GTest::gtest_main
    OpenGL::GL
    GLEW::GLEW
    glfw
    glm::glm
    Boost::boost
    Boost::system
)

# Add tests
add_test(NAME CppLiquidTests COMMAND CppLiquidTests)