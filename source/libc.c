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

  char buffer[256];
  char aux[32];

  strcpy(buffer, "\033[312mError: "); // Red text
  //strcpy(buffer, "");
  
  switch (errno) {
    case 0:
      strcpy(buffer, "No error."); break;
    case ESRCH: // 3
      strcat(buffer, "No such process."); break;
    case ENXIO: // 6
      strcat(buffer, "No such device or address."); break;
    case EBADF: // 9
      strcat(buffer, "Bad file number."); break;
    case EAGAIN: // 11
      strcat(buffer, "Try again."); break;
    case ENOMEM: // 12
      strcat(buffer, "Out of memory."); break;
    case EACCES: // 13
      strcat(buffer, "Permission denied."); break;
    case EFAULT: // 14
      strcat(buffer, "Bad address."); break;
    case ENODEV: // 19
      strcat(buffer, "No such device."); break;
    case EINVAL: // 22
      strcat(buffer, "Invalid argument."); break;
    case ENFILE: // 23
      strcat(buffer, "File table overflow."); break;
    case EMFILE: // 24
      strcat(buffer, "Too many open files."); break;
    case ENOTTY: // 25
      strcat(buffer, "Not a typewriter."); break;
    case ENOSYS: // 38
      strcat(buffer, "Function not implemented."); break;
    default:
      strcat(buffer, "Unknown error. Error number is ");
      itoa(errno, aux);
      strcat(buffer, aux);
      strcat(buffer, ".");
      break;
  }
  strcat(buffer, "\033[0m\n");
  //strcat(buffer, "\033[0m"); Escape code at the end does not work well.
  write(1, buffer, strlen(buffer));
}


///
///    FANCY CONSOLE OUTPUT
///

// print line
void printl (char* buff) {
  char new_buff[1024];
  strcpy(new_buff, buff);
  strcat(new_buff, "\n");
  int ret = write(1, new_buff, strlen(buff));
  if(ret < 0) perror();
}


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
  while ((*str1++ = *str2++));
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

// String compare
int strequ(const char* a, const char* b) {
  while (*a && *b) {
    if (*a != *b) return 0;
    a++;
    b++;
  }
  return (*a == *b);
}







