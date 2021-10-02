cmake_minimum_required(VERSION 3.0)
project(particle)
set (CMAKE_CXX_STANDARD 20)

find_package(MPI REQUIRED)
add_definitions(-DOMPI_SKIP_MPICXX)
link_libraries(MPI::MPI_C)
include_directories(./include)

add_executable(particle examples/particle.cpp)
add_executable(particle_tuple examples/particle_tuple.cpp)
