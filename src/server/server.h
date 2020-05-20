
#ifndef SERVER_H
#define SERVER_H

#define N_THREADS 4
#define SBUF_SIZE 16

#define MAXLINE 1024

int open_server_listen_sock(int port);
void serve_client(int connected_client);
void* server_thread(void* vargp);

#endif