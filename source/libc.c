/*
 * libc.c 
 */

#include <libc.h>
#include <types.h>

int errno;

// Syscalls

void perror() {
  char err[8];
  if (errno != 0) {
    itoa(errno, &err);
    char* buffer = strcat("An error occurred. Error number ", err);
    write(1, buffer, strlen(buffer));
  }
}




// library

void itoa(int a, char *b) {
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0) {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++) {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a) {
  int i;
  i=0;
  while (a[i]!=0) i++;
  return i;
}

char* strcat(char* str1, const char* str2) {
  char* ret = str1;
  while (*str1) str1++;
  while (*str1++ = *str2++);
  return ret;
}

