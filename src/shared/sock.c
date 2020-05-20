#include "sock.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

void send_sock(int fd, const void* buf, size_t n, int flags) {
    if (send(fd, buf, n, flags) < 0) {
        fprintf(stdout, "error: sock (%d) send message\n", fd);
        exit(0);
    }
}
