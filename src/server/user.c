
#include "user.h"

#include <mysql/mysql.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared/protocol.h"
#include "shared/utils.h"
#include "sql.h"

extern MYSQL G_mysql_connection;

// --------------------------- online_users 的操作函数---------------------------------
void online_users_init(OnlineUsers *online_users, int n) {
    online_users->user_socks = calloc(n, sizeof(UserSock *));
    online_users->n = n;
    online_users->front = online_users->rear = 0;
}

UserSock *online_users_search(OnlineUsers *online_users, uint32_t user_id) {
    int pos = online_users->front;
    int n = online_users->n;
    int rear = online_users->rear;
    while ((++pos) % n <= rear) {
        if ((online_users->user_socks[pos % n])->id == user_id) {
            return online_users->user_socks[pos % n];
        }
    }
    return NULL;
}
void online_users_insert(OnlineUsers *online_users, UserSock *user_sock) {
    online_users->user_socks[(++(online_users->rear)) % (online_users->n)] = user_sock;
}

// --------------------------- 用户登录 ---------------------------
// 从数据库中读取用户id和密码，成功返回true。
bool user_sign_in(uint32_t user_id, char *password) {
    int res;
    MYSQL_RES *result;
    char stmt_str[SQL_STMT_STR_LEN];

    sprintf(stmt_str, "select * from user where user_id=%d and user_password=\"%s\"", user_id, password);  // 从数据库中读取user_id对应的密码
    res = mysql_query(&G_mysql_connection, stmt_str);
    if (res != 0) {
        debug_print("error: server, user_sign_in, mysql qeury error code: %d\n", res);
        return false;
    }
    result = mysql_store_result(&G_mysql_connection);
    uint32_t num_rows = mysql_num_rows(result);

    mysql_free_result(result);
    return num_rows == 1;
}

int user_sign_up(uint32_t user_id, ProtocolBodySignUp *protocol_body) {
    int res;
    char stmt_str[SQL_STMT_STR_LEN];

    // 直接插入数据，通过返回值判断成功与否。
    sprintf(stmt_str, "insert into user (user_id, user_name, user_password) values (%d, \"%s\", \"%s\")", user_id, protocol_body->username, protocol_body->password);
    debug_print("%s\n", stmt_str);
    res = mysql_query(&G_mysql_connection, stmt_str);
    if (res != 0) {
        debug_print("error: server, user_sign_up, mysql qeury error code: %d\n", res);
        return 1;
    }
    return 0;
}