#include <libc.h>

int pid;

// include addAsm Header
int addAsm (int arg1, int arg2);

int add (int arg1, int arg2) {
	return arg1 + arg2;
}

int __attribute__ ((__section__(".text.main"))) main(void) {
  /* Next line, tries to move value 0 to CR3 register. 
     This register is a privileged one, and so it will raise an exception
     __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
  
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

  int counter = 0;
  while(1) {
    counter = addAsm(counter, 1);
  }
  
}
