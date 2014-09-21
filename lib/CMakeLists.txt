add_library(taricha SHARED taricha512.c taricha2_512.c taricha.h taricha512_common.h)

set(TARICHA_BIG_ENDIAN FALSE CACHE BOOL "Whether this is built for a big-endian processor")

if (TARICHA_BIG_ENDIAN)
	add_definitions(-DTARICHA_BIG_ENDIAN)
endif (TARICHA_BIG_ENDIAN)

install(TARGETS taricha DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(FILES taricha.h DESTINATION "${CMAKE_INSTALL_PREFIX}/include")
