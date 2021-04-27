/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>


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
int strlen(char *buffer);
char* strcat(char* dest, const char* src);
char* strcpy(char* dest, const char* src);
int strequ(const char* a, const char* b);

#endif  /* __LIBC_H__ */
