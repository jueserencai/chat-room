
#include "client.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

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
            fprintf(stdout, "receive message: \n%s\n", buf);
        }
    }
}
