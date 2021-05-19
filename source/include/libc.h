/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>

#define TEXT_BLACK \033[30m
#define TEXT_BLUE \033[31m
#define TEXT_GREEN \033[32m
#define TEXT_CYAN \033[33m
#define TEXT_RED \033[34m
#define TEXT_MAGENTA \033[35m
#define TEXT_BROWN \033[36m
#define TEXT_LIGHT_GRAY \033[37m
#define TEXT_DARK_GRAY \033[38m
#define TEXT_LIGHT_BLUE \033[39m
#define TEXT_LIGHT_GREEN \033[310m
#define TEXT_LIGHT_CYAN \033[311m
#define TEXT_LIGHT_RED \033[312m
#define TEXT_LIGHT_MAGENTA \033[313m
#define TEXT_YELLOW \033[314m
#define TEXT_WHITE \033[315m

#define BG_BLACK \033[40m
#define BG_BLUE \033[41m
#define BG_GREEN \033[42m
#define BG_CYAN \033[43m
#define BG_RED \033[44m
#define BG_MAGENTA \033[45m
#define BG_BROWN \033[46m
#define BG_LIGHT_GRAY \033[47m
#define BG_DARK_GRAY \033[48m
#define BG_LIGHT_BLUE \033[49m

// (Sysenter) SYSCALLS: Assembler Wrappers
int write(int fd, char *buffer, int size);
int gettime();
int getpid();
int fork();
void exit();
int get_stats(int pid, struct stats *s);

int open_tty_ro (int tty_fd);
int close(int fd);
int createScreen();

int setFocus(int fd);
int read(int fd, char* user_buff, int count);

// SYSCALLS: C wrappers
void perror();

// Fancy console output
void printl (char* buffer);

// Basic string treatment
void itoa(int num, char *buffer);
int atoi(char* buff);
int strlen(char* buffer);
char* strcat(char* dest, const char* src);
char* strcpy(char* dest, const char* src);
int strequ(const char* a, const char* b);

#endif  /* __LIBC_H__ */
