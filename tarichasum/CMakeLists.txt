add_executable(taricha512sum taricha512sum.c taricha_hash_stream.c tarichasum.c)
add_executable(taricha2_512sum taricha2_512sum.c taricha_hash_stream.c tarichasum.c)
target_link_libraries(taricha512sum taricha)
target_link_libraries(taricha2_512sum taricha)

install(TARGETS taricha512sum taricha2_512sum DESTINATION ${CMAKE_INSTALL_BINDIR})
