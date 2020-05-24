
#ifndef HEADER_H
#define HEADER_H

#define HEADER_TYPE_SIZE 20
#define HEADER_CONTENT_SIZE 50

void parse_header_line(char* line, char* type, char* content);

char* parse_header_command(char* headers_buf, char* command);

void construct_headers_sign_in(char* buf, char* username);
void parse_headers_sign_in(char* headers_buf, char* username);

void construct_headers_send_message(char* buf, char* from, char* to, char* message_type, char* message);
void parse_headers_send_message(char* headers_buf, char* from, char* to, char* message_type, char* message);

void construct_headers_user_offline(char* buf, char* username);
void parse_headers_user_offline(char* headers_buf, char* username, char* online_type);

#endif