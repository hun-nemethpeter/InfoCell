cmake_minimum_required(VERSION 3.27)

project(InfoCell)

# Conan integration
list(APPEND CMAKE_MODULE_PATH ${INFOCELL_CONAN_PATH_BASE}/${CMAKE_BUILD_TYPE})
list(APPEND CMAKE_PREFIX_PATH ${INFOCELL_CONAN_PATH_BASE}/${CMAKE_BUILD_TYPE})

set (CMAKE_CXX_STANDARD 20)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

find_package(nlohmann_json REQUIRED)
find_package(CLI11 REQUIRED)
find_package(utf8cpp REQUIRED)
find_package(ftxui REQUIRED)
find_package(GTest REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

enable_testing()

add_subdirectory(src)