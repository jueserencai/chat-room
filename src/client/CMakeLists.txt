file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)

find_package(Threads)

set(SOURCES client.h client.c user.h user.c global_variables.c)
add_executable(client_exe ${SOURCES} main.c)
add_library(client_lib STATIC ${SOURCES})

target_link_libraries(client_exe  shared_lib ${CMAKE_THREAD_LIBS_INIT} curses m)