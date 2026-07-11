cmake_minimum_required(VERSION 3.22) # Modern CMake version for SFML 3+
project(SignalScope)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable deprecation warnings from external dependencies to keep output clean
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

# 1. Automate SFML 3.1.0 Download and Compiling
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.1.0 
)

# Skip building SFML's optional examples and docs to speed up compile time
set(SFML_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(SFML_BUILD_DOC OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(SFML)

# 2. Define your project's executable source files
set(SOURCES 
    src/main.cpp 
    src/QuadTree.cpp 
    src/KDTree.cpp 
    src/MathUtils.cpp
)
add_executable(SignalScope ${SOURCES})

# 3. Include your local header files folder
target_include_directories(SignalScope PRIVATE include)

# 4. Link SFML targets (Note: SFML 3 targets use a clean namespace style)
target_link_libraries(SignalScope PRIVATE SFML::Graphics SFML::Window SFML::System)