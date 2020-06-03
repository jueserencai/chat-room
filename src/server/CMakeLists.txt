# file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)

find_package(Threads)
LINK_DIRECTORIES("/usr/lib/x86_64-linux-gnu/") # mysql lib path

set(SOURCES server.h server.c sql.h sql.c user.h user.c global_variables.c)
add_executable(server_exe ${SOURCES} main.c)
add_library(server_lib STATIC ${SOURCES})

target_link_libraries(server_exe shared_lib ${CMAKE_THREAD_LIBS_INIT} mysqlclient)
