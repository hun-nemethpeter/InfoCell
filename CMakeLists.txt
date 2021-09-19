project(ArcSolver)
cmake_minimum_required(VERSION 3.5)
set (CMAKE_CXX_STANDARD 20)
find_package(Boost 1.71)

include(../conan/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

enable_testing()
add_subdirectory(src)
