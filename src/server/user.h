
#ifndef SERVER_USER_H
#define SERVER_USER_H

#include "server.h"
#include "shared/user_info.h"

typedef struct {
    char name[USER_NAME_MAX_SIZE];
    int sock;
} UserSock;

typedef struct {
    UserSock** user_socks;
    int n;
    int front;
    int rear;
} OnlineUsers;

void online_users_init(OnlineUsers* online_users, int n);
UserSock* online_users_search(OnlineUsers* online_users, char* username);
void online_users_insert(OnlineUsers* online_users, UserSock* user_sock);

typedef enum  {
    SUCCEED,
    USER_OFFLINE
} SendMessageResultType;

#endif