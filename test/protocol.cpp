#include "gtest/gtest.h"

extern "C" {
#include "shared/protocol.h"
}

TEST(protocol, head) {
    char buf[1000];
    ProtocolHead source_protocol;
    source_protocol.version = PROTOCOL_VERSION;
    source_protocol.code = PROTOCOL_CODE_SIGN_IN;
    source_protocol.from_id = 100;
    source_protocol.to_id = 200;
    source_protocol.body_type = PROTOCOL_BODY_TYPE_SIGN_IN;
    protocol_head_encode(buf, &source_protocol);

    ProtocolHead parse_protocol;
    protocol_head_parse(buf, &parse_protocol);

    ASSERT_EQ(source_protocol.version, parse_protocol.version);
    ASSERT_EQ(source_protocol.code, parse_protocol.code);
    ASSERT_EQ(source_protocol.from_id, parse_protocol.from_id);
    ASSERT_EQ(source_protocol.to_id, parse_protocol.to_id);
    ASSERT_EQ(source_protocol.body_type, parse_protocol.body_type);
}