
#ifndef SERVER_H
#define SERVER_H

#include <stddef.h>

#include "user.h"

#define N_THREADS 4
#define SBUF_SIZE 16

#define MAXLINE 1024

int open_server_listen_sock(int port);
void serve_client(int connected_client);
void* server_thread(void* vargp);

SendMessageResultType push_message_to_user(char* username, char* message_type, char* message, size_t message_length);

void handle_request(int connected_client, char* request_buf, char* return_buf);

#endif