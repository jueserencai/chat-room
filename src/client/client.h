

#ifndef CLIENT_H
#define CLIENT_H

#include <ncurses.h>
#include <semaphore.h>

#define MAXLINE 1024

int open_client_sock(const char* hostname, int port);

void* client_wait_message_thread(void* vargp);

typedef struct {
    WINDOW* message_display_win;
    int message_display_height;
    int message_display_width;
    WINDOW* input_board_win;
    int input_board_height;
    int input_board_width;

    sem_t message_display_mutex;
} NcursesType;
void ncurses_init();
void ncurses_message_display(char* message);
void ncurses_clear_line(WINDOW* win, int y, int x);

#endif