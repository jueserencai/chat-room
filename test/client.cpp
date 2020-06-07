#include "gtest/gtest.h"

extern "C" {
#include "client/client.h"
#include "client/user.h"
}

TEST(client, user_sign_up) {
    int client_sock = open_client_sock("127.0.0.1", 6666);

    uint32_t user_id = 120;
    char username[] = "120 service";
    char password[] = "120";
    sign_up(client_sock, user_id, username, password);
}