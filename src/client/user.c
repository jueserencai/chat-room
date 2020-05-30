#include "user.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "client.h"
#include "shared/header.h"
#include "shared/protocol.h"
#include "shared/sock.h"
#include "shared/user_info.h"

void sign_in(int client_sock, uint32_t user_id, char *password)
{
    char buf[MAXLINE];
    ProtocolHead protocol_head;
    protocol_head.version = PROTOCOL_VERSION;
    protocol_head.code = PROTOCOL_CODE_SIGN_IN;
    protocol_head.from_id = user_id;
    protocol_head.body_type = PROTOCOL_BODY_TYPE_SIGN_IN;
    protocol_head_encode(buf, &protocol_head);

    ProtocolBodySignIn protocol_body;
    protocol_body.password_len = strlen(password);
    protocol_body.password = password;

    int body_size = protocol_body_encode(buf + PROTOCOL_HEAD_SIZE, &protocol_body, PROTOCOL_BODY_TYPE_SIGN_IN);

    send_sock(client_sock, buf, PROTOCOL_HEAD_SIZE + body_size, 0);
}