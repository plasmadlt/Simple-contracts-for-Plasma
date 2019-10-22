cmake_minimum_required(VERSION 3.5)

project(helllloworld VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ion.cdt CONFIG REQUIRED)
message("Found CDT package at: ${ION_CDT_ROOT}")

### Generate the wasm and abi
add_contract(helllloworld helllloworld hello.hpp hello.cpp)
