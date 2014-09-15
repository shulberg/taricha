add_library(taricha SHARED taricha512.c taricha2_512.c taricha.h taricha512_common.h)

install(TARGETS taricha DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(FILES taricha.h DESTINATION "${CMAKE_INSTALL_PREFIX}/include")
