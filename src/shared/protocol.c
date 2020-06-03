
#include "protocol.h"

#include <arpa/inet.h>
#include <string.h>

void protocol_head_encode(char *buf, ProtocolHead *protocol)
{
    uint8_t *p_data = buf;
    *p_data = protocol->version;
    p_data += 1;

    *p_data = protocol->code;
    p_data += 1;

    *(uint32_t *)p_data = htonl(protocol->from_id);
    p_data += 4;

    *(uint32_t *)p_data = htonl(protocol->to_id);
    p_data += 4;

    *p_data = protocol->body_type;
    p_data += 1;
}
void protocol_head_parse(char *buf, ProtocolHead *protocol)
{
    uint8_t *p_data = buf;
    protocol->version = *p_data;
    p_data += 1;

    protocol->code = *p_data;
    p_data += 1;

    protocol->from_id = ntohl(*(uint32_t *)p_data);
    p_data += 4;

    protocol->to_id = ntohl(*(uint32_t *)p_data);
    p_data += 4;

    protocol->body_type = *p_data;
    p_data += 1;
}


// 当前处理协议体的方式还是 逐个判断。先判断是哪个协议体，再按照对应的格式去处理。没有做一个统一的处理。后续添加协议体需要添加编码和解码的实现，比较麻烦。
ssize_t protocol_body_encode(char *buf, void *protocol_body, ProtocolBodyType body_type)
{
    uint8_t *p_data = buf;
    if (body_type == PROTOCOL_BODY_TYPE_SIGN_IN)
    {
        ProtocolBodySignIn *body = (ProtocolBodySignIn *)protocol_body;
        *p_data = body->password_len;
        p_data += 1;

        strncpy(p_data, body->password, body->password_len);
        p_data += body->password_len;
    }
    else if (body_type == PROTOCOL_BODY_TYPE_CHAT)
    {
        ProtocolBodyChatMessage *body = (ProtocolBodyChatMessage *)protocol_body;
        *p_data = body->type;
        p_data += 1;

        *p_data = body->format;
        p_data += 1;

        *(uint32_t *)p_data = htonl(body->len);
        p_data += 4;

        strncpy(p_data, body->content, body->len);
        p_data += body->len;
    }
    else if (body_type == PROTOCOL_BODY_TYPE_SERVER_STRING)
    {
        ProtocolBodyServerString *body = (ProtocolBodyServerString *)protocol_body;
        *p_data = body->len;
        p_data += 1;

        strncpy(p_data, body->content, body->len);
        p_data += body->len;
    }
    return (char *)p_data - buf;
}

void protocol_body_decode(char *buf, void *protocol_body, ProtocolBodyType body_type)
{
    uint8_t *p_data = buf;
    if (body_type == PROTOCOL_BODY_TYPE_SIGN_IN)
    {
        ProtocolBodySignIn *body = (ProtocolBodySignIn *)protocol_body;
        body->password_len = *p_data;
        p_data += 1;

        strncpy(body->password, p_data, body->password_len);
        p_data += body->password_len;
    }
    else if (body_type == PROTOCOL_BODY_TYPE_CHAT)
    {
        ProtocolBodyChatMessage *body = (ProtocolBodyChatMessage *)protocol_body;

        body->type = *p_data;
        p_data += 1;

        body->format = *p_data;
        p_data += 1;

        body->len = ntohl(*(uint32_t *)p_data);
        p_data += 4;

        strncpy(body->content, p_data, body->len);
        p_data += body->len;
    }
    else if (body_type == PROTOCOL_BODY_TYPE_SERVER_STRING)
    {
        ProtocolBodyServerString *body = (ProtocolBodyServerString *)protocol_body;
        body->len = *p_data;
        p_data += 1;

        strncpy(body->content, p_data, body->len);
        p_data += body->len;
    }
}