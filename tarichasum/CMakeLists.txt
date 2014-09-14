add_library(tarichasum STATIC taricha_hash_stream.c tarichasum.c options.c processing.c
                              taricha_hash_stream.h tarichasum.h options.h processing.h)

add_executable(taricha512sum taricha512sum.c)
add_executable(taricha2_512sum taricha2_512sum.c)

if (WIN32 AND NOT CYGWIN)

set(REGEX_INCLUDE_DIRECTORY F:/pcre/include CACHE PATH "Path containing regex.h")
set(REGEX_LIB F:/pcre/lib/pcreposix.lib CACHE FILEPATH "Regex library")

include_directories(${REGEX_INCLUDE_DIRECTORY})
target_link_libraries(tarichasum ${REGEX_LIB})

endif (WIN32 AND NOT CYGWIN)

target_link_libraries(tarichasum taricha)

target_link_libraries(taricha512sum tarichasum)
target_link_libraries(taricha2_512sum tarichasum)

install(TARGETS taricha512sum taricha2_512sum DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
