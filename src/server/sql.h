

#ifndef SERVER_SQL_H
#define SERVER_SQL_H

#include <mysql/mysql.h>

#define SQL_STMT_STR_LEN 300

void mysql_connect(MYSQL *mysql_connection);
void mysql_close(MYSQL *mysql_connection);

#endif