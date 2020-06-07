# chat-room
聊天室。纯C语言实现。


# 依赖库：
Ncurses 客户端字符界面库。当前只实现了，在终端中实现输入框和接受消息框分开，避免接受的消息和输入的字符混乱在一起。

mysql 服务端使用mysql存用户表，需要mysqlclient库，连接mysql服务。

googletest 只用于测试。 

除了上述库以及posix标准的p_thread线程库，没有其他第三方库。应用层的二进制协议自己实现。
