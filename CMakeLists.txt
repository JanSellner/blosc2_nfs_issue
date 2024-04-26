cmake_minimum_required(VERSION 3.12)
project(blosc_mmap)

set(CMAKE_CXX_STANDARD 17)

find_package(Blosc2 2.14.4 CONFIG REQUIRED)

add_executable(blosc_mmap main.cpp)
target_link_libraries(blosc_mmap PRIVATE Blosc2::blosc2_shared)

add_executable(blosc_mmap_test test_mmap.cpp)
target_link_libraries(blosc_mmap_test PRIVATE Blosc2::blosc2_shared)
