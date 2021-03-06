/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>

int write(int fd, char *buffer, int size); // uses sysenter
int write_deprecated(int fd, char *buffer, int size); // uses int
int gettime();

int getpid();
int fork();
void exit();

void itoa(int a, char *b);
int strlen(char *a);
char* strcat(char* str1, const char* str2);
char* strcpy(char* dest, const char* src);

#endif  /* __LIBC_H__ */
