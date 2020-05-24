
#include "header.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "user_info.h"

void parse_header_line(char* line, char* type, char* content) {
    int equal_pos = 0;
    while (line[equal_pos] != '=') {
        equal_pos++;
    }
    strncpy(type, line, equal_pos);
    strcpy(content, line + equal_pos + 1);
}

// 读取header第一行的 command 命令
// 返回第二行header的char指针
char* parse_header_command(char* headers_buf, char* command) {
    int command_end = -1;
    while (headers_buf[++command_end] != '\n')
        ;
    headers_buf[command_end] = '\0';
    char header_type[HEADER_TYPE_SIZE];  // header 类型，等号前面的部分
    parse_header_line(headers_buf, header_type, command);
    return headers_buf + command_end + 1;
}

// 当请求是 登录(sign_in) 时：
//     header部分格式为：
//         command=sign_in
//         user_name=xxx
//     还没没有实现注册，密码验证等
void construct_headers_sign_in(char* buf, char* username) {
    sprintf(buf, "command=sign_in\n");
    sprintf(buf, "%suser_name=%s\n", buf, username);
}
void parse_headers_sign_in(char* headers_buf, char* username) {
    char header_type[HEADER_TYPE_SIZE];  // header 类型，等号前面的部分
    char* token = NULL;
    token = strtok(headers_buf, "\n");
    parse_header_line(token, header_type, username);
}

// 当请求是 发送消息(send_message) 时
//     header部分格式为：
//         command=send_message
//         from=xxx              (user_name)
//         to=xxx                (user_name)
//         message_type=xxx      (string, jpg,... 当前实现了 string)
//         message_length=x      (数字， int 类型)
//                               (空一行，header部分结束)
//         xxxxxxxx              (message 的具体内容)
//     header的每一行后面为回车 "\n" 符。
void construct_headers_send_message(char* buf, char* from, char* to, char* message_type, char* message) {
    sprintf(buf, "command=send_message\n");
    sprintf(buf, "%sfrom=%s\n", buf, from);
    sprintf(buf, "%sto=%s\n", buf, to);
    sprintf(buf, "%smessage_type=%s\n", buf, message_type);
    sprintf(buf, "%smessage_length=%zu\n", buf, strlen(message));
    sprintf(buf, "%s\n%s", buf, message);
}

void parse_headers_send_message(char* headers_buf, char* from, char* to, char* message_type, char* message) {
    char* token = NULL;
    char header_type[HEADER_TYPE_SIZE];  // header 类型，等号前面的部分

    token = strtok(headers_buf, "\n");
    parse_header_line(token, header_type, from);

    token = strtok(NULL, "\n");
    parse_header_line(token, header_type, to);

    token = strtok(NULL, "\n");
    parse_header_line(token, header_type, message_type);

    char message_length_str[HEADER_CONTENT_SIZE];
    token = strtok(NULL, "\n");
    parse_header_line(token, header_type, message_length_str);
    int message_length = atoi(message_length_str);

    token = strtok(NULL, "\n");  // header头后面的空行
    strncpy(message, token, message_length + 1);
}

void construct_headers_user_offline(char* buf, char* username) {
    sprintf(buf, "command=user_online_type\n");
    sprintf(buf, "%susername=%s\n", buf, username);
    sprintf(buf, "%sonline_type=offline\n", buf);
}
void parse_headers_user_offline(char* headers_buf, char* username, char* online_type) {
    char* token = NULL;
    char header_type[HEADER_TYPE_SIZE];  // header 类型，等号前面的部分

    token = strtok(headers_buf, "\n");
    parse_header_line(token, header_type, username);

    token = strtok(NULL, "\n");
    parse_header_line(token, header_type, online_type);
}
