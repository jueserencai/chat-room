
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

void handle_request(int connected_client, char* request_buf);

void reply_client_string(int client_sock, uint32_t user_id, char* content);

#endif