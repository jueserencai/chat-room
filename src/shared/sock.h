

#ifndef SHARED_SOCK_H
#define SHARED_SOCK_H

#include <sys/types.h>

#define HEADER_TYPE_SIZE 20
#define HEADER_CONTENT_SIZE 50

// sock
void send_sock(int fd, const void* buf, size_t n, int flags);

#endif