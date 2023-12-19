# A null exemple for API checking.
cmake_minimum_required(VERSION 3.16)
project(base)

add_executable(base
    ${CMAKE_CURRENT_SOURCE_DIR}/../../htk.cpp
	base.cpp
)