cmake_minimum_required(VERSION 3.16)

project(BHCollision
    VERSION 1.0.0
    DESCRIPTION "Binary Black Hole Collision Simulator"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# GLM (header-only math library, shared with the renderer)
# ============================================================================
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8
)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG        glew-cmake-2.2.0
)
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(ONLY_LIBS ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glm glfw glew)

# ============================================================================
# Core simulation library (static)
# ============================================================================
set(LIB_SOURCES
    src/physics.cpp
    src/integrator.cpp
    src/merger.cpp
    src/simulation.cpp
    src/integration_api.cpp
)

add_library(bh_collision_lib STATIC ${LIB_SOURCES})
target_include_directories(bh_collision_lib PUBLIC include)
target_link_libraries(bh_collision_lib PUBLIC glm::glm)

# MSVC: enable M_PI, M_E etc. from <cmath>
target_compile_definitions(bh_collision_lib PUBLIC _USE_MATH_DEFINES)

# ============================================================================
# Main executable
# ============================================================================
add_executable(bh_collision src/main.cpp)
target_link_libraries(bh_collision PRIVATE bh_collision_lib)

# ============================================================================
# Test executable
# ============================================================================
add_executable(bh_collision_tests tests/test_physics.cpp)
target_link_libraries(bh_collision_tests PRIVATE bh_collision_lib)

# Enable testing
enable_testing()
add_test(NAME PhysicsTests COMMAND bh_collision_tests)

# ============================================================================
# 3D Viewer executable (OpenGL)
# ============================================================================
add_executable(bh_viewer src/viewer.cpp)
target_link_libraries(bh_viewer PRIVATE bh_collision_lib glfw libglew_static opengl32)

# ============================================================================
# Output directories
# ============================================================================
set_target_properties(bh_collision bh_collision_tests bh_viewer
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Create output directory for simulation data
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/output")
