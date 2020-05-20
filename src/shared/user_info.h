
#ifndef SHARED_USER_INFO_H
#define SHARED_USER_INFO_H

#define USER_NAME_MAX_SIZE 50
#define USER_PASSWORD_MAX_SIZE 20

struct user_info {
    char name[USER_NAME_MAX_SIZE];
    char password[USER_PASSWORD_MAX_SIZE];
};

#endif