
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client.h"
#include "shared/header.h"
#include "shared/protocol.h"
#include "shared/sock.h"
#include "shared/user_info.h"
#include "user.h"

NcursesType ncurses_data;

int main(int argc, char *argv[])
{
    // 初始化ncurses窗口界面
    ncurses_init(&ncurses_data);
    WINDOW *input_board_win = ncurses_data.input_board_win;

    int client_sock;
    char user_name[USER_NAME_MAX_SIZE];
    char command[100];
    uint32_t user_id;

    while (1)
    {
        mvwprintw(input_board_win, 0, 0, "command: ");
        wgetstr(input_board_win, command);
        ncurses_clear_line(input_board_win, 0, 9 + strlen(command)); // 清空上次命令字符串的残留

        if (strcmp(command, "connect") == 0)
        {
            char send_sock_buf[MAXLINE];
            client_sock = open_client_sock("127.0.0.1", 6666);
            if (client_sock > 0)
            {
                ncurses_message_display("connected to server");
            }
        }
        else if (strcmp(command, "sign_in") == 0)
        {
            char user_id_str[USER_ID_MAX_SIZE];
            char *end;
            char password[USER_PASSWORD_MAX_SIZE];

            mvwprintw(input_board_win, 1, 0, "input your id: ");
            wgetstr(input_board_win, user_id_str);
            user_id = strtoul(user_id_str, &end, 10);
            if (end >= user_id_str + USER_ID_MAX_SIZE)
            {
                ncurses_message_display("error: user id too long");
                break;
            }

            mvwprintw(input_board_win, 2, 0, "input your password: ");
            wgetstr(input_board_win, password);

            sign_in(client_sock, user_id, password);

            pthread_t tid; // 等待接收消息的线程
            pthread_create(&tid, NULL, client_wait_message_thread, &client_sock);
        }
        else if (strcmp(command, "chat") == 0)
        {
            char to_user_id_str[USER_ID_MAX_SIZE];
            uint32_t to_user_id;
            char *end;
            char message[MAXLINE / 2];
            char send_buf[MAXLINE];

            mvwprintw(input_board_win, 1, 0, "chat to: ");
            wgetstr(input_board_win, to_user_id_str);
            ncurses_clear_line(input_board_win, 1, strlen(to_user_id_str) + 9);
            to_user_id = strtoul(to_user_id_str, &end, 10);
            if (end >= to_user_id_str + USER_ID_MAX_SIZE)
            {
                ncurses_message_display("error: user id too long");
                break;
            }

            while (1)
            {
                mvwprintw(input_board_win, 2, 0, "message: ");
                wgetstr(input_board_win, message);
                if (strcmp(message, "/quit") == 0)
                {
                    ncurses_clear_line(input_board_win, 2, 14); // 清空上次消息的残留
                    break;
                }

                size_t message_len = strlen(message);
                if (message_len < 1)
                    continue;
                ncurses_clear_line(input_board_win, 2, 9 + message_len); // 清空上次消息的残留

                ProtocolHead protocol_head;
                protocol_head.version = PROTOCOL_VERSION;
                protocol_head.code = PROTOCOL_CODE_CHAT;
                protocol_head.from_id = user_id;
                protocol_head.to_id = to_user_id;
                protocol_head.body_type = PROTOCOL_BODY_TYPE_CHAT;
                protocol_head_encode(send_buf, &protocol_head);

                ProtocolBodyChatMessage protocol_body;
                protocol_body.type = PROTOCOL_CHAT_MESSAGE_STRING;
                protocol_body.len = message_len;
                protocol_body.content = message;
                ssize_t body_size = protocol_body_encode(send_buf + PROTOCOL_HEAD_SIZE, &protocol_body, PROTOCOL_BODY_TYPE_CHAT);

                send_sock(client_sock, send_buf, PROTOCOL_HEAD_SIZE + body_size, 0);

                sprintf(send_buf, "to %d: %s", to_user_id, message); // 自己发送的消息也显示在接收窗口中
                ncurses_message_display(send_buf);
            }
        }
    }

    endwin(); /* End curses mode		  */

    close(client_sock);
    return 0;
}