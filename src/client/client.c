
#include "client.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "shared/header.h"
#include "shared/user_info.h"

int open_client_sock(char* hostname, int port) {
    int client_sock;
    struct sockaddr_in server_addr;

    // 创建客户端sock
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stdout, "error: create client socket\n");
        exit(1);
    }

    // 设置服务器socket参数
    server_addr.sin_family = AF_INET;
    inet_aton(hostname, &(server_addr.sin_addr));
    server_addr.sin_port = htons(port);

    // 连接服务器
    if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stdout, "error: connect server\n");
        exit(1);
    }
    return client_sock;
}

void* client_wait_message_thread(void* vargp) {
    pthread_detach(pthread_self());

    char buf[MAXLINE];
    int client_sock = *(int*)vargp;
    while (1) {
        int n = recv(client_sock, buf, sizeof(buf), 0);

        if (n > 0) {
            // 读取第一行的 command 命令
            int command_end = -1;
            while (buf[++command_end] != '\n')
                ;
            buf[command_end] = '\0';
            char header_type[HEADER_TYPE_SIZE];                   // header 类型，等号前面的部分
            char header_content[HEADER_CONTENT_SIZE];             // header 具体内容，等号后面的部分
            parse_header_line(buf, header_type, header_content);  // 第一行 为请求命令的类型。根据请求命令类型分别处理。
            char* headers_buf = buf + command_end + 1;

            if (strcmp(header_content, "send_message") == 0) {
                char from_user[USER_NAME_MAX_SIZE];
                char to_user[USER_NAME_MAX_SIZE];
                char message_type[HEADER_CONTENT_SIZE];
                char message[MAXLINE];

                parse_headers_send_message(headers_buf, from_user, to_user, message_type, message);
                if (strcmp(message_type, "string") == 0) {
                    fprintf(stdout, "from %s: %s\n", from_user, message);
                }
            } else if (strcmp(header_content, "user_online_type") == 0) {
                char username[USER_PASSWORD_MAX_SIZE];
                char online_type[HEADER_CONTENT_SIZE];

                parse_headers_user_offline(headers_buf, username, online_type);
                fprintf(stdout, "user %s %s\n", username, online_type);
            }
        }
    }
}
