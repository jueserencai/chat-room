file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)

add_library(shared_lib STATIC ${SOURCES})