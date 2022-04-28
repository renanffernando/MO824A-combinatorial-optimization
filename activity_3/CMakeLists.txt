cmake_minimum_required(VERSION 3.0.0)
project(concorde_tsp_cpp)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Werror -Wextra -Wpedantic -pedantic-errors -fmax-errors=1")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(
    concorde_solver
    ${CMAKE_CURRENT_SOURCE_DIR}/src/concordeSolver.cpp
)

target_link_libraries(
    concorde_solver
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/concorde.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/qsopt.a
)

add_library(
    solve_z
    ${CMAKE_CURRENT_SOURCE_DIR}/src/solve_z.cpp
)

add_executable(
    concorde_solve_instance
    ${CMAKE_CURRENT_SOURCE_DIR}/src/concorde_solve_instance.cpp
)

target_link_libraries(
    concorde_solve_instance
    concorde_solver
)

add_executable(
    concorde_example
    ${CMAKE_CURRENT_SOURCE_DIR}/src/concorde_example.cpp
)

target_link_libraries(
    concorde_example
    concorde_solver
)

add_executable(
    z_solver_test
    ${CMAKE_CURRENT_SOURCE_DIR}/src/z_solver_test.cpp
)

target_link_libraries(
    z_solver_test
    solve_z
)
