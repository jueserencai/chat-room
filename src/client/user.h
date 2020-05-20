
#ifndef CLIENT_USER_H
#define CLIENT_USER_H

#include <stdbool.h>

bool sign_up();
bool sign_in(int client_sock, char* username);

#endif