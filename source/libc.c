/*
 * libc.c 
 */

#include <libc.h>
#include <types.h>
#include <errno.h>

int errno;

///
///    SYSCALLS
///

void perror() {

  char buffer[128];
  char err[8];

  switch (errno) {
    case 0:
      strcpy(buffer, "No error.\n");
      write(1, buffer, strlen(buffer));
      break;

    case 14:
      strcpy(buffer, "Bad address.\n");
      write(1, buffer, strlen(buffer));
      break;

    case 22:
      strcpy(buffer, "Invalid argument.\n");
      write(1, buffer, strlen(buffer));
      break;

    case 38:
      strcpy(buffer, "Function not implemented.\n");
      write(1, buffer, strlen(buffer));
      break;

    default:
      strcpy(buffer, "Unknown error. Error number is ");
      itoa(errno, err);
      strcat(buffer, err);
      strcat(buffer, ".\n");
      write(1, buffer, strlen(buffer));
      break;
  }
}


///
///    FANCY CONSOLE OUTPUT
///

// PRINTF


///
///    BASIC STRING TREATMENT
///

// Integer to string
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

// String length
int strlen(char *a) {
  int i;
  i=0;
  while (a[i]!=0) i++;
  return i;
}

// Concatenate strings
char* strcat(char* str1, const char* str2) {
  char* ret = str1;
  while (*str1) str1++;
  while (*str1++ = *str2++);
  return ret;
}

// String copy
char* strcpy(char* d, const char* s) {
  if (d == NULL) return NULL; 
  char* ptr = d;
  while (*s != 0) {
    *d = *s;
    d++;
    s++;
  }
  *d = '\0';
  return ptr;
}







