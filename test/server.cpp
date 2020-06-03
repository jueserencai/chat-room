#include "gtest/gtest.h"

extern "C" {
#include "server/sql.h"
#include "server/user.h"
}

extern MYSQL G_mysql_connection;  // mysql 连接，用于从mysql服务器查询数据

TEST(mysql, connect) {
    mysql_connect(&G_mysql_connection);
}

TEST(user, sign_in) {
    // mysql_connect(&G_mysql_connection);

    uint32_t user_id = 101;
    char password[20] = "wxn";
    ASSERT_TRUE(user_sign_in(user_id, password));

    user_id = 101;
    sprintf(password, "wzf");
    ASSERT_FALSE(user_sign_in(user_id, password));
}