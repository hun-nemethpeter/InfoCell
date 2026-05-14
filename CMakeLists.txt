cmake_minimum_required(VERSION 3.27)

project(InfoCell)

# Conan integration
include(conan/${CMAKE_BUILD_TYPE}/conan_cmakedeps_paths.cmake)

set (CMAKE_CXX_STANDARD 20)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

find_package(nlohmann_json CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(utf8cpp CONFIG REQUIRED)
find_package(ftxui CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

enable_testing()

add_subdirectory(src)