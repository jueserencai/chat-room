
#include "protocol.h"

#include <arpa/inet.h>

void protocol_head_encode(char* buf, ProtocolHead* protocol) {
    uint8_t* p_data = buf;
    *p_data = protocol->version;
    p_data += 1;

    *p_data = protocol->code;
    p_data += 1;

    *(uint32_t*)p_data = htonl(protocol->from_id);
    p_data += 4;

    *(uint32_t*)p_data = htonl(protocol->to_id);
    p_data += 4;

    *p_data = protocol->body_type;
    p_data += 1;
}
void protocol_head_parse(char* buf, ProtocolHead* protocol) {
    uint8_t* p_data = buf;
    protocol->version = *p_data;
    p_data += 1;

    protocol->code = *p_data;
    p_data += 1;

    protocol->from_id = ntohl(*(uint32_t*)p_data);
    p_data += 4;

    protocol->to_id = ntohl(*(uint32_t*)p_data);
    p_data += 4;

    protocol->body_type = *p_data;
    p_data += 1;
}
