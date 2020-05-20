
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "shared/sbuf.h"
#include "shared/sock.h"
#include "shared/user_info.h"
#include "user.h"

Sbuf sbuf;                 // sock缓冲区管理
OnlineUsers online_users;  // 在线user信息，包括 name，sock

int main(int argc, char* argv[]) {
    fprintf(stdout, "server starting\n");
    int listen_sock = open_server_listen_sock(6666);
    fprintf(stdout, "start succeed\n");

    int connected_sock;
    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);

    pthread_t tid;

    sbuf_init(&sbuf, SBUF_SIZE);
    for (int i = 0; i < N_THREADS; i++) {
        pthread_create(&tid, NULL, server_thread, NULL);
    }

    online_users_init(&online_users, SBUF_SIZE);

    while (1) {
        connected_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &client_len);
        if (connected_sock < 0) {
            fprintf(stdout, "error: server accpet\n");
            exit(0);
        }
        sbuf_insert(&sbuf, connected_sock);
    }

    return 0;
}