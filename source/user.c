#include <libc.h>

int pid;

int __attribute__ ((__section__(".text.main"))) main(void) {
  
  char buff[1024];
  
  // TEST 1
  strcpy(buff, "\n## -> TEST 1 : a normal write()\n");
  if ( write(1, buff, strlen(buff)) < 0 ) perror();
  strcpy(buff, "Well, in fact this is a normal write()\n");
  if ( write(1, buff, strlen(buff)) < 0 ) perror();
  
  // TEST 2
  strcpy(buff, "\n## -> TEST 2 : write() with an 'invalid' file descriptor\n");
  if ( write(1, buff, strlen(buff)) < 0 ) perror();
  if ( write(22, buff, -1) < 0 ) perror(); // Invalid fd
  
  // TEST 3
  strcpy(buff, "\n## -> TEST 3 : write() with an invalid size\n");
  if ( write(1, buff, strlen(buff)) < 0 ) perror();
  if ( write(1, buff, -1) < 0 ) perror(); // Invalid size
  
  // TEST 4
  strcpy(buff, "\n## -> TEST 4 : gettime()\n");
  if ( write(1, buff, strlen(buff)) < 0 ) perror();
  strcpy(buff, "Number of ticks is ");
  if ( write(1, buff, strlen(buff))  < 0 ) perror();
  itoa(gettime(), buff);
  strcat(buff, "\n");
  if ( write(1, buff, strlen(buff))  < 0 ) perror();

  // TEST 5 (interactive)
  strcpy(buff, "\n## -> TEST 5 : press any key.\n");
  if ( write(1, buff, strlen(buff)) < 0 ) perror();
  strcpy(buff, "The screen can scroll down (if you press enough keys..)\n");
  if ( write(1, buff, strlen(buff)) < 0 ) perror();

  // TEST 6
  strcpy(buff, "\n## -> TEST 6 : getpid()\n");
  strcat(buff, "My PID is ");
  if ( write(1, buff, strlen(buff))  < 0 ) perror();
  itoa(getpid(), buff);
  strcat(buff, "\n");
  if ( write(1, buff, strlen(buff))  < 0 ) perror();

  int ret = fork();
  if (ret == 0) { // FILL
    int last = 0;
    while(1) {
      int now = gettime();
      if (now > last) {
        last = now;
        itoa(now, buff);
        int my_pid = getpid();
        strcpy(buff, "< FILL > Tick=");  write(1, buff, strlen(buff));
        itoa(now, buff);
        strcat(buff, "; PID="); write(1, buff, strlen(buff));
        itoa(my_pid, buff);     
        strcat(buff, "\n");     write(1, buff, strlen(buff));
      }
    }
  }
  else { // PARE
    int last = 0;
    while(1) {
      int now = gettime();
      if (now > last) {
        last = now;
        itoa(now, buff);
        int my_pid = getpid();
        strcpy(buff, "< ADAM > Tick=");  write(1, buff, strlen(buff));
        itoa(now, buff);        
        strcat(buff, "; PID="); write(1, buff, strlen(buff));
        itoa(my_pid, buff);     
        strcat(buff, "\n");     write(1, buff, strlen(buff));
      }
    }
  }
}



