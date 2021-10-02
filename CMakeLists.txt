cmake_minimum_required(VERSION 3.0)
project(particle)
set (CMAKE_CXX_STANDARD 20)

find_package(MPI REQUIRED)
add_definitions(-DOMPI_SKIP_MPICXX)
link_libraries(MPI::MPI_C)

add_executable(particle examples/particle.cpp)
target_include_directories(particle PUBLIC ./include)