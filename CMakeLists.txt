cmake_minimum_required(VERSION 3.22)
project(SignalScope)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
)

set(SFML_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(SFML_BUILD_DOC OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(SFML)
set(SOURCES 
    src/main.cpp 
    src/QuadTree.cpp 
    src/KDTree.cpp 
    src/MathUtils.cpp
)
add_executable(SignalScope ${SOURCES})
target_include_directories(SignalScope PRIVATE include)
target_link_libraries(SignalScope PRIVATE sfml-graphics sfml-window sfml-system)