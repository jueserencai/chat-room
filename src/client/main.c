
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client.h"
#include "shared/header.h"
#include "shared/sock.h"
#include "shared/user_info.h"
#include "user.h"

int main(int argc, char* argv[]) {
    char send_buf[MAXLINE];
    int client_sock = open_client_sock("127.0.0.1", 6666);
    fprintf(stdout, "connected to server\n");

    // 登录用户的名字
    char my_user_name[USER_NAME_MAX_SIZE];
    fprintf(stdout, "input your name: ");
    fgets(my_user_name, sizeof(my_user_name), stdin);
    my_user_name[strlen(my_user_name) - 1] = '\0';
    sign_in(client_sock, my_user_name);

    // 等待接收消息的线程
    pthread_t tid;
    pthread_create(&tid, NULL, client_wait_message_thread, &client_sock);

    // 聊天好友的名字
    char to_user[USER_NAME_MAX_SIZE];
    fprintf(stdout, "which one do you want to chat: ");
    fgets(to_user, sizeof(to_user), stdin);
    to_user[strlen(to_user) - 1] = '\0';

    // fprintf(stdout, "to %s: ", to_user);
    char message[500];
    while (fgets(message, sizeof(message), stdin) != NULL) {
        construct_headers_send_message(send_buf, my_user_name, to_user, "string", message);
        send_sock(client_sock, send_buf, sizeof(send_buf), 0);
        // fprintf(stdout, "to %s: ", to_user);
    }

    close(client_sock);
    return 0;
}