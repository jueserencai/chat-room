
#include "user.h"

#include <stdlib.h>
#include <string.h>

void online_users_init(OnlineUsers* online_users, int n) {
    online_users->user_socks = calloc(n, sizeof(UserSock*));
    online_users->n = n;
    online_users->front = online_users->rear = 0;
}

UserSock* online_users_search(OnlineUsers* online_users, char* username) {
    int pos = online_users->front;
    int n = online_users->n;
    int rear = online_users->rear;
    while ((++pos) % n <= rear) {
        if (strcmp((online_users->user_socks[pos % n])->name, username) == 0) {
            return online_users->user_socks[pos % n];
        }
    }
    return NULL;
}
void online_users_insert(OnlineUsers* online_users, UserSock* user_sock) {
    online_users->user_socks[(++(online_users->rear)) % (online_users->n)] = user_sock;
}