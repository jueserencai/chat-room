
#include "client.h"

#include <arpa/inet.h>
#include <math.h>
#include <ncurses.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "shared/header.h"
#include "shared/user_info.h"

extern NcursesType ncurses_data;

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

    char recv_buf[MAXLINE];
    char display_buf[MAXLINE];
    int client_sock = *(int*)vargp;
    while (1) {
        int n = recv(client_sock, recv_buf, sizeof(recv_buf), 0);

        if (n > 0) {
            // 读取第一行的 command 命令
            int command_end = -1;
            while (recv_buf[++command_end] != '\n')
                ;
            recv_buf[command_end] = '\0';
            char header_type[HEADER_TYPE_SIZE];                        // header 类型，等号前面的部分
            char header_content[HEADER_CONTENT_SIZE];                  // header 具体内容，等号后面的部分
            parse_header_line(recv_buf, header_type, header_content);  // 第一行 为请求命令的类型。根据请求命令类型分别处理。
            char* headers_buf = recv_buf + command_end + 1;

            if (strcmp(header_content, "send_message") == 0) {
                char from_user[USER_NAME_MAX_SIZE];
                char to_user[USER_NAME_MAX_SIZE];
                char message_type[HEADER_CONTENT_SIZE];
                char message[MAXLINE];

                parse_headers_send_message(headers_buf, from_user, to_user, message_type, message);
                if (strcmp(message_type, "string") == 0) {
                    sprintf(display_buf, "from %s: %s", from_user, message);
                }
            } else if (strcmp(header_content, "user_online_type") == 0) {
                char username[USER_NAME_MAX_SIZE];
                char online_type[HEADER_CONTENT_SIZE];

                parse_headers_user_offline(headers_buf, username, online_type);
                sprintf(display_buf, "user %s %s", username, online_type);
            }

            ncurses_message_display(display_buf);
        }
    }
}

void ncurses_init(NcursesType* ncurses) {
    initscr();
    cbreak();
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    ncurses->message_display_height = max_y / 10 * 6;
    ncurses->message_display_width = max_x;
    ncurses->message_display_win = newwin(ncurses->message_display_height, ncurses->message_display_width, 0, 0);
    scrollok(ncurses->message_display_win, TRUE);

    ncurses->input_board_height = max_y - ncurses->message_display_height;
    ncurses->input_board_width = max_x;
    ncurses->input_board_win = newwin(ncurses->input_board_height, ncurses->input_board_width, ncurses->message_display_height, 0);
    refresh();
}

void ncurses_message_display(char* message) {
    int scroll_lines = ceil(strlen(message) * 1.0 / ncurses_data.message_display_width);
    wscrl(ncurses_data.message_display_win, scroll_lines);
    mvwprintw(ncurses_data.message_display_win,
              ncurses_data.message_display_height - scroll_lines - 1,
              0,
              message);
    wrefresh(ncurses_data.message_display_win);
}