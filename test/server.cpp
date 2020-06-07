#include "gtest/gtest.h"

extern "C" {
#include "server/sql.h"
#include "server/user.h"
}

extern MYSQL G_mysql_connection;  // mysql 连接，用于从mysql服务器查询数据

TEST(mysql, connect) {
    mysql_connect(&G_mysql_connection);
}

TEST(server, user_sign_in) {
    uint32_t user_id = 101;
    char password[20] = "wxn";
    ASSERT_TRUE(user_sign_in(user_id, password));

    user_id = 101;
    sprintf(password, "wzf");
    ASSERT_FALSE(user_sign_in(user_id, password));
}

TEST(server, user_sign_up) {
    uint32_t user_id = 114;
    char username[USER_NAME_MAX_SIZE] = "114 service";
    char password[USER_PASSWORD_MAX_SIZE] = "114";

    ProtocolBodySignUp protocol_body;
    protocol_body.username = username;
    protocol_body.username_len = strlen(username);
    protocol_body.password = password;
    protocol_body.password_len = strlen(password);

    ASSERT_EQ(user_sign_up(user_id, &protocol_body), 0);
}