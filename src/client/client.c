
#include "client.h"

#include <arpa/inet.h>
#include <math.h>
#include <ncurses.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "shared/header.h"
#include "shared/protocol.h"
#include "shared/user_info.h"

extern NcursesType ncurses_data;

int open_client_sock(char *hostname, int port)
{
    int client_sock;
    struct sockaddr_in server_addr;

    // 创建客户端sock
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        ncurses_message_display("error: create client socket");
        return -1;
    }

    // 设置服务器socket参数
    server_addr.sin_family = AF_INET;
    inet_aton(hostname, &(server_addr.sin_addr));
    server_addr.sin_port = htons(port);

    // 连接服务器
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        ncurses_message_display("error: connect server");
        return -1;
    }
    return client_sock;
}

void *client_wait_message_thread(void *vargp)
{
    pthread_detach(pthread_self());

    char recv_buf[MAXLINE];
    char display_buf[MAXLINE];
    int client_sock = *(int *)vargp;

    ProtocolHead protocol_head;

    while (1)
    {
        int n = recv(client_sock, recv_buf, sizeof(recv_buf), 0);

        if (n < 1)
            continue;

        protocol_head_parse(recv_buf, &protocol_head);

        char *body_buf = recv_buf + PROTOCOL_HEAD_SIZE;

        if (protocol_head.code == PROTOCOL_CODE_CHAT)
        {
            ProtocolBodyChatMessage protocol_body;
            protocol_body.content = display_buf;

            protocol_body_decode(body_buf, &protocol_body, PROTOCOL_BODY_TYPE_CHAT);

            if (protocol_body.type == PROTOCOL_CHAT_MESSAGE_STRING)
            {
                display_buf[protocol_body.len] = '\0';
                sprintf(recv_buf, "from %d: %s", protocol_head.from_id, display_buf);
                ncurses_message_display(recv_buf);
            }
        }
        else if (protocol_head.code == PROTOCOL_CODE_SERVER_STRING)
        {
            ProtocolBodyServerString protocol_body;
            protocol_body.content = display_buf;
            protocol_body_decode(body_buf, &protocol_body, PROTOCOL_BODY_TYPE_SERVER_STRING);

            display_buf[protocol_body.len] = '\0';
            ncurses_message_display(display_buf);
        }
    }
}

// 初始化 ncurses 界面
void ncurses_init(NcursesType *ncurses)
{
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

    sem_init(&(ncurses_data.message_display_mutex), 0, 1);

    refresh();
}

// 在 消息接收窗口中显示message信息。
// message放在窗口的最后一行，窗口向上滚动
// 消息接收窗口 的最后一行不显示字符，和 下面的 消息输入窗口 分开
void ncurses_message_display(char *message)
{
    sem_wait(&(ncurses_data.message_display_mutex));
    int scroll_lines = ceil(strlen(message) * 1.0 / ncurses_data.message_display_width);
    wscrl(ncurses_data.message_display_win, scroll_lines);
    mvwprintw(ncurses_data.message_display_win,
              ncurses_data.message_display_height - scroll_lines - 1,
              0,
              message);
    wrefresh(ncurses_data.message_display_win);
    sem_post(&(ncurses_data.message_display_mutex));
}

// 将window的第y行，第x列后面的字符清空
void ncurses_clear_line(WINDOW *win, int y, int x)
{
    wmove(win, y, x);
    wclrtoeol(win);
}