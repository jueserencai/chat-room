
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client.h"
#include "shared/sock.h"
#include "user.h"

int main(int argc, char* argv[]) {
    char buf[1024];
    fprintf(stdout, "client\n");
    int client_sock = open_client_sock("127.0.0.1", 6666);
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        send_sock(client_sock, buf, sizeof(buf), 0);
        int n = recv(client_sock, buf, sizeof(buf), 0);
        if (n > 0) {
            fprintf(stdout, "receive message: %s\n", buf);
        }
    }

    close(client_sock);
    return 0;
}