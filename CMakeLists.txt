cmake_minimum_required(VERSION 3.0)
project(particle)
set (CMAKE_CXX_STANDARD 20)

find_package(MPI REQUIRED)
add_definitions(-DOMPI_SKIP_MPICXX)
link_libraries(MPI::MPI_C)
include_directories(./include)

add_executable(particles_struct examples/particles_struct.cpp)
add_executable(single_particle_struct examples/single_particle_struct.cpp)
add_executable(particles_tuple examples/particles_tuple.cpp)
add_executable(single_tuple examples/single_tuple.cpp)
