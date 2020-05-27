
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

typedef enum {
    PROTOCOL_CODE_CHAT = 1,
    PROTOCOL_CODE_CHAT_GROUP = 2,
    PROTOCOL_CODE_SIGN_IN = 6,
    PROTOCOL_CODE_SIGN_UP = 14
} ProtocolCode;

// 协议头格式
//
// code字段根据以下值，进行不同的处理
// 私聊       0x01
// 群聊       0x02,
// 文件       0x03,
// 登录       0x06,
// 成功       0x07,
// 失败       0x08,
// 用户id请求 0x0C,
// 群组id请求 0x0D,
// 加入私聊   0x0E,
// 加入群聊   0x0F,
// 退出私聊   0x10,
// 退出群聊   0x11
// 好友列表   0x12
// 群组列表   0x13
// 个人注册   0x14
typedef struct {
    uint8_t version;    // 版本
    uint8_t code;       // 控制字段
    uint32_t from_id;   // 源用户id
    uint32_t to_id;     // 目的用户id
    uint8_t body_type;  // 协议体类型，比如是ProtocolMessage等
} ProtocolHead;
#define PROTOCOL_VERSION 1
void protocol_head_encode(char* buf, ProtocolHead* protocol);
void protocol_head_parse(char* buf, ProtocolHead* protocol);

typedef enum {
    PROTOCOL_BODY_TYPE_MESSAGE = 1,
    PROTOCOL_BODY_TYPE_SIGN_IN = 6,
} ProtocolBodyType;

// 协议体中 注册 登录 的内容格式
// body_type=1
typedef struct {
    uint8_t username_len;
    char* username;

    uint8_t password_len;
    char* password;
} ProtocolBodySign;

// 协议体中 消息 格式
// body_type=2
// 消息类型有：
// 字符串      1,
// 图片        2
// 语音        3
// 文件        4
typedef struct {
    uint8_t type;    // 消息类型
    uint8_t format;  // 消息格式，比如字符串消息需要有编码格式，图片需要有格式jpg，png等等
    uint32_t len;    // 消息长度
} ProtocolBodyMessage;

#endif