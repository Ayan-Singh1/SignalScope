cmake_minimum_required(VERSION 3.10)

# Project Name
project(SignalScope)

# Use C++ 17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the headers from the "include" folder
include_directories(include)

# (OPTIONAL FOR LATER): Find SFML package 
# We commented this out for Day 1 so your team can compile basic C++ 
# even if they haven't installed SFML on their computers yet.
# find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Add all .cpp files in the src folder to the executable
file(GLOB SOURCES "src/*.cpp")

# Create the executable named SignalScope
add_executable(SignalScope ${SOURCES}
    include/Tower.h)

# (OPTIONAL FOR LATER): Link SFML to the executable
# target_link_libraries(SignalScope sfml-graphics sfml-window sfml-system)

# -----------------------------------------------------------------------
# Standalone test target for the KD-Tree + RF math (Hitesh).
# Builds ONLY the KD-Tree, math, and its test main -- no SFML/ImGui.
# Build just this:  cmake --build . --target kdtree_test
# -----------------------------------------------------------------------
add_executable(kdtree_test
    tests/kdtree_test.cpp
    src/KDTree.cpp
    src/MathUtils.cpp)