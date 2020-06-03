

#include "sql.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mysql_connect(MYSQL *mysql_connection) {
    if (mysql_real_connect(mysql_connection, "127.0.0.1", "chat_room_root", "chat_room_root", "chat_room", 3306, NULL, 0) == NULL) {
        fprintf(stderr, "error: connect to mysql server error\n");
    }
}

void mysql_close(MYSQL *mysql_connection) {
    mysql_close(mysql_connection);
}
