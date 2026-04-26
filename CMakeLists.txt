cmake_minimum_required(VERSION 3.14)
project(NetworkTopology LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(network_topology STATIC
    src/network_topology.cpp
)
target_include_directories(network_topology PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_executable(main_app src/main.cpp)
target_link_libraries(main_app network_topology)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_executable(test_topology tests/test_network_topology.cpp)
target_link_libraries(test_topology
    network_topology
    gtest_main
)
target_include_directories(test_topology PRIVATE include)