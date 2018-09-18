#!cmake .
# This works with SystemV versions 2.3.2 and later, which have support for
# cmake and specify SystemC_CXX_STANDARD.
# 
# REQUIREMENTS:
#   Define SYSTEMC_HOME to point to install directory of SystemC (not source)

cmake_minimum_required(VERSION 3.11)

project(engine CXX)

include(systemc.cmake)

add_compile_definitions(SC_INCLUDE_FX SC_INCLUDE_DYNAMIC_PROCESSES)

add_executable(engine+.x engine+.cpp)
add_executable(engine.x engine.cpp)
#target_compile_features(engine.x PUBLIC cxx_std_11)
#target_include_directories(engine.x includes)
#target_link_libraries(engine.x ${CMAKE_SOURCE_DIR}/lib{:YOUR_STATIC_LIB:}.a)
