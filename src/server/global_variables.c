
#include "shared/sbuf.h"
#include "sql.h"
#include "user.h"

Sbuf G_sbuf;                 // sock缓冲区管理
OnlineUsers G_online_users;  // 在线user信息
MYSQL G_mysql_connection;    // mysql 连接，用于从mysql服务器查询数据