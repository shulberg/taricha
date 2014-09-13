add_library(tarichasum STATIC taricha_hash_stream.c tarichasum.c taricha_hash_stream.h tarichasum.h)

add_executable(taricha512sum taricha512sum.c)
add_executable(taricha2_512sum taricha2_512sum.c)

target_link_libraries(tarichasum taricha)

target_link_libraries(taricha512sum tarichasum)
target_link_libraries(taricha2_512sum tarichasum)

install(TARGETS taricha512sum taricha2_512sum DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
