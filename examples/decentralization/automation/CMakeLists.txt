cmake_minimum_required(VERSION 3.16)
project(base)

add_executable(base
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../htk.cpp
	main.cpp
)

# # Add devtools
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../tools/devtools devtools_build)
# target_link_libraries(base PUBLIC devtools)