cmake_minimum_required (VERSION 2.6)

project (taricha)

if (CMAKE_COMPILER_IS_GNUCC)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
endif (CMAKE_COMPILER_IS_GNUCC)

set(EXECUTABLE_OUTPUT_PATH bin)
set(LIBRARY_OUTPUT_PATH bin)

add_subdirectory(lib)
add_subdirectory(tarichasum)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

