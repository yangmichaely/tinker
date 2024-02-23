cmake_minimum_required(VERSION 3.0.0)
project(Tinker VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(assembler assembler.c)
add_executable(simulator simulator.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
