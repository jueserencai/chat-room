file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)

find_package(Threads)
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)


add_executable(client_exe ${SOURCES})
add_library(client_lib STATIC ${SOURCES})

target_link_libraries(client_exe shared_lib ${CMAKE_THREAD_LIBS_INIT} ${CURSES_LIBRARIES} m)