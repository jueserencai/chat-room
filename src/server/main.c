
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "shared/sbuf.h"
#include "shared/sock.h"
#include "shared/user_info.h"
#include "sql.h"
#include "user.h"

extern Sbuf G_sbuf;                 // sock缓冲区管理
extern OnlineUsers G_online_users;  // 在线user信息
extern MYSQL G_mysql_connection;    // mysql 连接，用于从mysql服务器查询数据

int main(int argc, char* argv[]) {
    // -------------------开启监听服务-------------------
    fprintf(stdout, "server starting\n");
    int listen_sock = open_server_listen_sock(6666);
    fprintf(stdout, "start succeed\n");

    int connected_sock;
    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);

    // -------------------初始化-------------------
    pthread_t tid;
    sbuf_init(&G_sbuf, SBUF_SIZE);
    for (int i = 0; i < N_THREADS; i++) {
        pthread_create(&tid, NULL, server_thread, NULL);
    }
    online_users_init(&G_online_users, SBUF_SIZE);
    mysql_connect(&G_mysql_connection);

    // -------------------服务开始运行-------------------
    while (1) {
        connected_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &client_len);
        if (connected_sock < 0) {
            fprintf(stdout, "error: server accpet\n");
        }
        sbuf_insert(&G_sbuf, connected_sock);
    }

    return 0;
}