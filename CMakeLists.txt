cmake_minimum_required (VERSION 2.6)

project (taricha)

add_executable(taricha512sum taricha512sum.c taricha_hash_stream.c tarichasum.c)
add_executable(taricha2_512sum taricha2_512sum.c taricha_hash_stream.c tarichasum.c)
add_library(taricha SHARED taricha512.c taricha2_512.c)
target_link_libraries(taricha512sum taricha)
target_link_libraries(taricha2_512sum taricha)

set(EXECUTABLE_OUTPUT_PATH bin)
set(LIBRARY_OUTPUT_PATH bin)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

install(TARGETS taricha512sum taricha2_512sum DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS taricha DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES taricha.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
