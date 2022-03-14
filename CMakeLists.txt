cmake_minimum_required(VERSION 3.12)
project(matrix_multiplication_parallel)

# HPX
find_package(HPX REQUIRED)

set (program par_matrix_mult)
add_hpx_executable(${program} ESSENTIAL SOURCES ${program}.cpp)
