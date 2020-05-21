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
#include "shared/sbuf.h"
#include "shared/sock.h"
#include "shared/user_info.h"
#include "user.h"

extern Sbuf sbuf;
extern OnlineUsers online_users;

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
    if (bind(listen_sock, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) < 0) {
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

void* server_thread(void* vargp) {
    pthread_detach(pthread_self());
    while (1) {
        int connected_client = sbuf_remove(&sbuf);
        serve_client(connected_client);
        close(connected_client);
    }
}

void serve_client(int connected_client) {
    char recv_buf[MAXLINE];
    fprintf(stdout, "serve client: sock %d\n", connected_client);
    int n = recv(connected_client, recv_buf, sizeof(recv_buf), 0);
    while (n > 0) {
        fprintf(stdout, "received message from sock %d: \n%s\n", connected_client, recv_buf);
        handle_request(connected_client, recv_buf);
        n = recv(connected_client, recv_buf, sizeof(recv_buf), 0);
    }
}

// 处理请求的字符串 函数
// 根据 request_buf 里面的字符串内容，处理请求。然后将回复消息放在 return_buf中。

void handle_request(int connected_client, char* request_buf) {
    // 读取第一行的 command 命令
    int command_end = -1;
    while (request_buf[++command_end] != '\n')
        ;
    request_buf[command_end] = '\0';
    char header_type[HEADER_TYPE_SIZE];                           // header 类型，等号前面的部分
    char header_content[HEADER_CONTENT_SIZE];                     // header 具体内容，等号后面的部分
    parse_header_line(request_buf, header_type, header_content);  // 第一行 为请求命令的类型。根据请求命令类型分别处理。
    request_buf += command_end + 1;                               // 从 command_end 后面开始解析headers

    if (strcmp(header_content, "sign_in") == 0) {
        char username[USER_NAME_MAX_SIZE];
        parse_headers_sign_in(request_buf, username);

        UserSock* user_sock = malloc(sizeof(UserSock));
        strcpy(user_sock->name, username);
        user_sock->sock = connected_client;
        online_users_insert(&online_users, user_sock);
    } else if (strcmp(header_content, "send_message") == 0) {
        char from_user[USER_NAME_MAX_SIZE];
        char to_user[USER_NAME_MAX_SIZE];
        char message_type[HEADER_CONTENT_SIZE];
        char message[MAXLINE];

        parse_headers_send_message(request_buf, from_user, to_user, message_type, message);
        UserSock* user_sock = online_users_search(&online_users, to_user);
        char send_buf[MAXLINE];
        if (user_sock == NULL) {
            construct_headers_user_offline(send_buf, to_user);
            send_sock(connected_client, send_buf, sizeof(send_buf), 0);
        } else {
            construct_headers_send_message(send_buf, from_user, to_user, message_type, message);
            send_sock(user_sock->sock, send_buf, sizeof(send_buf), 0);
        }
    }
}
