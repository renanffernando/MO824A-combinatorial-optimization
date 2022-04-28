cmake_minimum_required(VERSION 3.0.0)
project(concorde_tsp_cpp)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Werror -Wextra -Wpedantic -pedantic-errors -fmax-errors=1")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    /opt/gurobi951/linux64/include
)

link_directories(
    /opt/gurobi951/linux64/lib
)

add_executable(
    concorde_example
    ${CMAKE_CURRENT_SOURCE_DIR}/src/concorde_example.cpp
)

target_link_libraries(
  concorde_example
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/concorde.a
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/qsopt.a
)

add_executable(
    ktsp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ktsp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subTourElim.cpp
)

target_link_libraries(
    ktsp
    m
    gurobi_c++
    gurobi95
)
