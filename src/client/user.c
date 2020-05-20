#include "user.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "client.h"
#include "shared/sock.h"
#include "shared/user_info.h"

bool sign_up() {
    char buf[MAXLINE];
    char user_input[MAXLINE];
    int client_sock = open_client_sock("127.0.0.1", 6666);
    sprintf(buf, "/sign_up");

    fprintf(stdout, "sign up\n");

    fprintf(stdout, "input your name: \n");
    fgets(user_input, USER_NAME_MAX_SIZE, stdin);
    sprintf(buf, "%s %s", buf, user_input);

    fprintf(stdout, "input your password: \n");
    fgets(user_input, USER_PASSWORD_MAX_SIZE, stdin);
    sprintf(buf, "%s %s", buf, user_input);

    send_sock(client_sock, buf, sizeof(buf), 0);
    ssize_t recv_num = recv(client_sock, buf, sizeof(buf), 0);
    if (recv_num > 0 && strcmp(buf, "succeed") == 0) {
        return true;
    } else {
        return false;
    }
    close(client_sock);
}

bool sign_in(int client_sock, char* username) {
    char send_buf[MAXLINE];
    sprintf(send_buf, "command=sign_in\n");
    sprintf(send_buf, "%suser_name=%s\n", send_buf, username);
    send_sock(client_sock, send_buf, sizeof(send_buf), 0);
}