cmake_minimum_required (VERSION 2.6)

project (taricha)

set(EXECUTABLE_OUTPUT_PATH bin)
set(LIBRARY_OUTPUT_PATH bin)

add_subdirectory(lib)
add_subdirectory(tarichasum)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

