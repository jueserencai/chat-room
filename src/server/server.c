#include "server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "shared/header.h"
#include "shared/protocol.h"
#include "shared/sbuf.h"
#include "shared/sock.h"
#include "shared/user_info.h"
#include "user.h"

extern Sbuf G_sbuf;
extern OnlineUsers G_online_users;

int open_server_listen_sock(int port) {
    int listen_sock;
    struct sockaddr_in listen_addr;
    // 创建服务器监听sock
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stdout, "error: create server listen sock\n");
        exit(1);
    }

    // 服务器 监听addr设置
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_addr.sin_port = htons(port);

    // bind
    if (bind(listen_sock, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
        fprintf(stdout, "error: server bind\n");
        exit(1);
    }

    // listen
    if (listen(listen_sock, 1024) < 0) {
        fprintf(stdout, "error: server listen\n");
        exit(1);
    }

    return listen_sock;
}

void *server_thread(void *vargp) {
    pthread_detach(pthread_self());
    while (1) {
        int connected_client = sbuf_remove(&G_sbuf);
        serve_client(connected_client);
        close(connected_client);
    }
}

void serve_client(int connected_client) {
    char recv_buf[MAXLINE];
    fprintf(stdout, "serve client: sock %d\n", connected_client);
    int n = recv(connected_client, recv_buf, sizeof(recv_buf), 0);
    while (n > 0) {
        fprintf(stdout, "\nreceived message from sock %d\n", connected_client);
        handle_request(connected_client, recv_buf);
        n = recv(connected_client, recv_buf, sizeof(recv_buf), 0);
    }
}

// 处理请求的字符串 函数
// 根据 request_buf 里面的字符串内容，处理请求。
void handle_request(int connected_client, char *request_buf) {
    ProtocolHead protocol_head;
    protocol_head_parse(request_buf, &protocol_head);

    fprintf(stdout, "from user id: %d\n", protocol_head.from_id);
    fprintf(stdout, "request type code: %d\n", protocol_head.code);

    char *body_buf = request_buf + PROTOCOL_HEAD_SIZE;

    if (protocol_head.code == PROTOCOL_CODE_SIGN_IN) {
        char password[USER_PASSWORD_MAX_SIZE];
        ProtocolBodySignIn protocol_body;
        protocol_body.password = password;

        // 解析协议体
        protocol_body_decode(body_buf, &protocol_body, PROTOCOL_BODY_TYPE_SIGN_IN);

        // 验证用户密码
        if (user_sign_in(protocol_head.from_id, protocol_body.password)) {
            // 登录成功，加入在线用户列表
            UserSock *user_sock = malloc(sizeof(UserSock));
            user_sock->id = protocol_head.from_id;
            user_sock->sock = connected_client;
            online_users_insert(&G_online_users, user_sock);
        } else {
            // 密码错误
            char content[] = "wrong password";
            reply_client_string(connected_client, protocol_head.from_id, content);
        }

    } else if (protocol_head.code == PROTOCOL_CODE_CHAT) {
        UserSock *user_sock = online_users_search(&G_online_users, protocol_head.to_id);
        if (user_sock == NULL)  // 接受消息的用户 to_id 不在线。回复一个 用户离线 的消息
        {
            char content[] = "user offline";
            reply_client_string(connected_client, protocol_head.from_id, content);
        } else  // 用户在线，直接转发
        {
            send_sock(user_sock->sock, request_buf, MAXLINE, 0);  // MAXLINE 这个参数还可以计算优化。
        }
    }
}

void reply_client_string(int client_sock, uint32_t user_id, char *content) {
    char send_buf[MAXLINE];

    ProtocolHead server_string_head;
    server_string_head.version = PROTOCOL_VERSION;
    server_string_head.code = PROTOCOL_CODE_SERVER_STRING;
    server_string_head.from_id = user_id;
    server_string_head.body_type = PROTOCOL_BODY_TYPE_SERVER_STRING;
    protocol_head_encode(send_buf, &server_string_head);

    ProtocolBodyServerString server_string_body;
    server_string_body.len = strlen(content);
    server_string_body.content = content;
    ssize_t body_size = protocol_body_encode(send_buf + PROTOCOL_HEAD_SIZE, &server_string_body, PROTOCOL_BODY_TYPE_SERVER_STRING);

    send_sock(client_sock, send_buf, PROTOCOL_HEAD_SIZE + body_size, 0);
}

// void handle_request(int connected_client, char* request_buf) {
//     char command[HEADER_CONTENT_SIZE];
//     request_buf = parse_header_command(request_buf, command);
//     char send_buf[MAXLINE];

//     if (strcmp(command, "sign_in") == 0) {
//         char username[USER_NAME_MAX_SIZE];
//         parse_headers_sign_in(request_buf, username);

//         UserSock* user_sock = malloc(sizeof(UserSock));
//         strcpy(user_sock->name, username);
//         user_sock->sock = connected_client;
//         online_users_insert(&online_users, user_sock);
//     } else if (strcmp(command, "send_message") == 0) {
//         char from_user[USER_NAME_MAX_SIZE];
//         char to_user[USER_NAME_MAX_SIZE];
//         char message_type[HEADER_CONTENT_SIZE];
//         char message[MAXLINE];
//         parse_headers_send_message(request_buf, from_user, to_user, message_type, message);

//         UserSock* user_sock = online_users_search(&online_users, to_user);
//         if (user_sock == NULL) {
//             construct_headers_user_offline(send_buf, to_user);
//             send_sock(connected_client, send_buf, sizeof(send_buf), 0);
//         } else {
//             construct_headers_send_message(send_buf, from_user, to_user, message_type, message);
//             send_sock(user_sock->sock, send_buf, sizeof(send_buf), 0);
//         }
//     }
// }
