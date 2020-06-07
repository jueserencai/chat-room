

#ifdef DEBUG
#include <stdarg.h>
#include <stdio.h>

int debug_print(const char *format, ...) {
    va_list argPtr;
    int count;

    va_start(argPtr, format);                 /*  获取可变参数列表  */
    fflush(stdout);                           /*  强制刷新输出缓冲区  */
    count = vfprintf(stderr, format, argPtr); /*  将信息输出到标准出错流设备  */
    va_end(argPtr);                           /*  可变参数列表结束  */
}
#else
inline int debug_print(const char *format, ...) {
}
#endif
