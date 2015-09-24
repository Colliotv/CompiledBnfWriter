cmake_minimum_required(VERSION 3.3)
project(CompiledBnfWriter)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(includes/)
set(SOURCE_FILES src/launch.cpp includes/ includes/BNF/nodes.hh includes/BNF/varTables.hh includes/BNF/grammar.hpp includes/PP/string.hpp)
add_executable(CompiledBnfWriter ${SOURCE_FILES})