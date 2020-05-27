
#ifndef CLIENT_USER_H
#define CLIENT_USER_H

#include <stdbool.h>
#include <stdint.h>

void sign_in(int client_sock, uint32_t user_id, char* user_name, char* password);

#endif