cmake_minimum_required(VERSION 3.0.0)
project(concorde_tsp_cpp)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wpedantic -pedantic-errors -fmax-errors=1")

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
    main
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lagrangian.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/heuristic.cpp
)

target_link_libraries(
    main
    solve_z
    concorde_solver
)
