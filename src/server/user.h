
#ifndef SERVER_USER_H
#define SERVER_USER_H

#include "server.h"
#include "shared/user_info.h"

// 当前推送消息给用户的实现方法是：
//     用户登录后，建立一个用户登录信息，包括用户名字，建立连接的sock描述符。所有登录的用户信息放在一个循环队列里面，类似shared/sbuf包的实现。
//     当需要推送消息给用户时，查找这个用户的登录信息。
//     若找到了，则通过sock发送消息。
//     若没有找到，则回复 "用户离线" 信息给 请求发送消息 的这个用户

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
void online_users_remove(OnlineUsers* online_users, UserSock* user_sock);  // 未实现


// 服务器推送消息给用户的返回值，
typedef enum {
    SUCCEED,
    USER_OFFLINE
} SendMessageResultType;

#endif