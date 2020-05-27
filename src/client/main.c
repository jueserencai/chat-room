
#include <ncurses.h>
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

NcursesType ncurses_data;

int main(int argc, char* argv[]) {
    // 初始化ncurses窗口界面
    ncurses_init(&ncurses_data);
    WINDOW* input_board_win = ncurses_data.input_board_win;

    int client_sock;
    char user_name[USER_NAME_MAX_SIZE];
    char command[100];
    uint32_t user_id;

    while (1) {
        mvwprintw(input_board_win, 0, 0, "command: ");
        wgetstr(input_board_win, command);
        ncurses_clear_line(input_board_win, 0, 9 + strlen(command));  // 清空上次命令字符串的残留

        if (strcmp(command, "connect") == 0) {
            char send_sock_buf[MAXLINE];
            client_sock = open_client_sock("127.0.0.1", 6666);
            if (client_sock > 0) {
                ncurses_message_display("connected to server");
            }
        } else if (strcmp(command, "sign_in") == 0) {
            char password[USER_PASSWORD_MAX_SIZE];
            char user_id_str[11];
            char* end;
            mvwprintw(input_board_win, 1, 0, "input your id: ");
            wgetstr(input_board_win, user_id_str);
            user_id = strtoul(user_id_str, &end, 10);
            mvwprintw(input_board_win, 1, 0, "input your name: ");
            wgetstr(input_board_win, user_name);
            mvwprintw(input_board_win, 2, 0, "input your password: ");
            wgetstr(input_board_win, password);
            sign_in(client_sock, user_id, user_name, password);

            pthread_t tid;  // 等待接收消息的线程
            pthread_create(&tid, NULL, client_wait_message_thread, &client_sock);
        } else if (strcmp(command, "chat") == 0) {
            char to_user[USER_NAME_MAX_SIZE];
            char message[MAXLINE / 2];
            char send_sock_buf[MAXLINE];

            mvwprintw(input_board_win, 1, 0, "chat to: ");
            wgetstr(input_board_win, to_user);
            ncurses_clear_line(input_board_win, 1, 9 + strlen(to_user));

            while (1) {
                mvwprintw(input_board_win, 2, 0, "message: ");
                wgetstr(input_board_win, message);
                if (strcmp(message, "/quit") == 0) {
                    ncurses_clear_line(input_board_win, 2, 14);  // 清空上次消息的残留
                    break;
                }

                int message_len = strlen(message);
                if (message_len > 0) {
                    ncurses_clear_line(input_board_win, 2, 9 + message_len);  // 清空上次消息的残留

                    construct_headers_send_message(send_sock_buf, user_name, to_user, "string", message);  // 构造header发送给服务器
                    send_sock(client_sock, send_sock_buf, sizeof(send_sock_buf), 0);

                    sprintf(send_sock_buf, "to %s: %s", to_user, message);  // 自己发送的消息也显示在接收窗口中
                    ncurses_message_display(send_sock_buf);
                }
            }
        }
    }

    endwin(); /* End curses mode		  */

    close(client_sock);
    return 0;
}