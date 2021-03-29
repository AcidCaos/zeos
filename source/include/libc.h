/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>


// SYSCALLS: Assembler Wrappers
int write(int fd, char *buffer, int size); // uses sysenter
int write_deprecated(int fd, char *buffer, int size); // uses int
int gettime();
int getpid();
// TODO 
int fork();
void exit();

// SYSCALLS: C wrappers
void perror();

// Fancy console output
void printl (char* buffer);

// Basic string treatment
void itoa(int num, char *buffer);
int strlen(char *buffer);
char* strcat(char* dest, const char* src);
char* strcpy(char* dest, const char* src);

#endif  /* __LIBC_H__ */
