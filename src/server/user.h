
#ifndef SERVER_USER_H
#define SERVER_USER_H

#include <stdbool.h>
#include <stdint.h>

#include "server.h"
#include "shared/protocol.h"
#include "shared/user_info.h"

// -------------------------------------用户在线实现-----------------------------------

// 当前推送消息给用户的实现方法是：
//     用户登录后，建立一个用户登录信息，包括用户名字、建立连接的sock描述符。所有登录的用户信息放在一个循环队列里面，类似shared/sbuf包的实现。
//     当需要推送消息给用户时，查找这个用户的登录信息。
//     若找到了，则通过sock发送消息。
//     若没有找到，则回复 "用户离线" 信息给 请求发送消息 的这个用户

typedef struct
{
    uint32_t id;
    int sock;
} UserSock;

typedef struct
{
    UserSock **user_socks;
    int n;
    int front;
    int rear;
} OnlineUsers;

void online_users_init(OnlineUsers *online_users, int n);
UserSock *online_users_search(OnlineUsers *online_users, uint32_t user_id);
void online_users_insert(OnlineUsers *online_users, UserSock *user_sock);
void online_users_remove(OnlineUsers *online_users, UserSock *user_sock);  // 未实现

// -------------------------------------用户登录等操作-----------------------------------
bool user_sign_in(uint32_t user_id, char *password);
int user_sign_up(uint32_t user_id, ProtocolBodySignUp *protocol_body);

#endif