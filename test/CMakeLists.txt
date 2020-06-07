set(BINARY unit_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)


add_executable(${BINARY} ${TEST_SOURCES})


target_link_libraries(${BINARY} PUBLIC gtest client_lib server_lib shared_lib curses mysqlclient)
