file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)

add_executable(client_exe ${SOURCES})
add_library(client_lib STATIC ${SOURCES})

target_link_libraries(client_exe shared_lib)