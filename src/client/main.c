
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

    // 连接服务器
    char send_buf[MAXLINE];
    int client_sock = open_client_sock("127.0.0.1", 6666);
    ncurses_message_display("connected to server");

    // 登录用户的名字
    char my_user_name[USER_NAME_MAX_SIZE];
    mvwprintw(input_board_win, 0, 0, "input your name: ");
    wgetstr(input_board_win, my_user_name);
    sign_in(client_sock, my_user_name);

    // 等待接收消息的线程
    pthread_t tid;
    pthread_create(&tid, NULL, client_wait_message_thread, &client_sock);

    char chat_user_name[USER_NAME_MAX_SIZE];
    char message[MAXLINE / 2];

    char prompt_chat_user[] = "chat to whom: ";
    int prompt_chat_user_len = strlen(prompt_chat_user);
    // 清空 登录时的输入残留
    wmove(input_board_win, 0, prompt_chat_user_len);
    wclrtoeol(input_board_win);
    while (1) {
        mvwprintw(input_board_win, 0, 0, prompt_chat_user);
        wmove(input_board_win, 0, prompt_chat_user_len + 1);
        wrefresh(input_board_win);
        wgetstr(input_board_win, chat_user_name);

        mvwprintw(input_board_win, 1, 0, "to %s: ", chat_user_name);
        // 清空好友名字后面残留的字符
        wmove(input_board_win, 0, prompt_chat_user_len);
        wclrtoeol(input_board_win);
        // 清空输入的消息
        wmove(input_board_win, 1, 5 + strlen(chat_user_name));
        wclrtobot(input_board_win);
        wrefresh(input_board_win);

        // 接收新的消息
        wgetstr(input_board_win, message);

        construct_headers_send_message(send_buf, my_user_name, chat_user_name, "string", message);
        send_sock(client_sock, send_buf, sizeof(send_buf), 0);
    }

    endwin(); /* End curses mode		  */

    close(client_sock);
    return 0;
}