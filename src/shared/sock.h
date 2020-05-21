

#ifndef SHARED_SOCK_H
#define SHARED_SOCK_H

#include <sys/types.h>

// sock
void send_sock(int fd, const void* buf, size_t n, int flags);

#endif