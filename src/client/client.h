

#ifndef CLIENT_H
#define CLIENT_H

#define MAXLINE 1024

int open_client_sock(char* hostname, int port);

void* client_wait_message_thread(void* vargp);

#endif