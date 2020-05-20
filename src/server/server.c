#include "server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "shared/sbuf.h"
#include "shared/sock.h"

extern sbuf_t sbuf;

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

void serve_client(int connected_client) {
    char recv_buf[MAXLINE];
    char send_buf[MAXLINE];
    fprintf(stdout, "serve client: sock %d\n", connected_client);
    int n = recv(connected_client, recv_buf, sizeof(recv_buf), 0);
    while (n > 0) {
        fprintf(stdout, "received message from sock %d: %s\n", connected_client, recv_buf);
        sprintf(send_buf, "server recieved message: %s", recv_buf);
        send_sock(connected_client, send_buf, sizeof(send_buf), 0);
        n = recv(connected_client, recv_buf, sizeof(recv_buf), 0);
    }
}

void* server_thread(void* vargp) {
    pthread_detach(pthread_self());
    while (1) {
        int connected_client = sbuf_remove(&sbuf);
        serve_client(connected_client);
        close(connected_client);
    }
}