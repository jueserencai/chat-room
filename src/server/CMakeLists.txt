file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)

find_package(Threads)

add_executable(server_exe ${SOURCES})
add_library(server_lib STATIC ${SOURCES})

target_link_libraries(server_exe shared_lib ${CMAKE_THREAD_LIBS_INIT})
