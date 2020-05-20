#include "server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "shared/sbuf.h"
#include "shared/sock.h"
#include "shared/user_info.h"
#include "shared/utils.h"
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
    char send_buf[MAXLINE];
    fprintf(stdout, "serve client: sock %d\n", connected_client);
    int n = recv(connected_client, recv_buf, sizeof(recv_buf), 0);
    while (n > 0) {
        fprintf(stdout, "received message from sock %d: \n%s\n", connected_client, recv_buf);
        handle_request(connected_client, recv_buf, send_buf);
        send_sock(connected_client, send_buf, sizeof(send_buf), 0);
        n = recv(connected_client, recv_buf, sizeof(recv_buf), 0);
    }
}

void handle_request(int connected_client, char* request_buf, char* return_buf) {
    char* token = NULL;
    token = strtok(request_buf, "\n");
    char header_type[HEADER_TYPE_SIZE];
    char header_content[HEADER_CONTENT_SIZE];
    parse_header_line(token, header_type, header_content);

    if (strcmp(header_content, "sign_in") == 0) {
        char username[USER_NAME_MAX_SIZE];
        token = strtok(NULL, "\n");
        parse_header_line(token, header_type, username);
        UserSock* user_sock = malloc(sizeof(UserSock));
        strcpy(user_sock->name, username);
        user_sock->sock = connected_client;
        online_users_insert(&online_users, user_sock);
    } else if (strcmp(header_content, "send_message") == 0) {
        char from_user[USER_NAME_MAX_SIZE];
        token = strtok(NULL, "\n");
        parse_header_line(token, header_type, from_user);

        char to_user[USER_NAME_MAX_SIZE];
        token = strtok(NULL, "\n");
        parse_header_line(token, header_type, to_user);

        char message_type[HEADER_CONTENT_SIZE];
        token = strtok(NULL, "\n");
        parse_header_line(token, header_type, message_type);

        char message_length_str[HEADER_CONTENT_SIZE];
        token = strtok(NULL, "\n");
        parse_header_line(token, header_type, message_length_str);
        int message_length = atoi(message_length_str);

        token = strtok(NULL, "\n");  // header头后面的空行
        char message[message_length + 1];
        strcpy(message, token);

        switch (push_message_to_user(to_user, message_type, message, message_length)) {
            case USER_OFFLINE:
                sprintf(return_buf, "user %s offline\n", to_user);
                break;
            default:
                sprintf(return_buf, "server push message to %s succeed\n", to_user);
                break;
        }
    }
}

SendMessageResultType push_message_to_user(char* username, char* message_type, char* message, size_t message_length) {
    UserSock* user_sock = online_users_search(&online_users, username);
    if (user_sock == NULL) {
        return USER_OFFLINE;
    }
    if (strcmp(message_type, "string") == 0) {
        send_sock(user_sock->sock, message, message_length, 0);
    }
    return SUCCEED;
}
