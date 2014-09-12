add_library(taricha SHARED taricha512.c taricha2_512.c)

install(TARGETS taricha DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES taricha.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
