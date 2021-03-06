#include <libc.h>

int pid;

// include addAsm Header
int addAsm (int arg1, int arg2);

int add (int arg1, int arg2) {
	return arg1 + arg2;
}

int __attribute__ ((__section__(".text.main"))) main(void) {
  /* Next line, tries to move value 0 to CR3 register. 
   * This register is a privileged one, and so it will raise an exception */
  /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
  
  char buff[1024];
  
  strcpy(buff, "Well, in fact this is a write()\n");
  if ( write(1, buff, strlen(buff)) < 0 ) perror();

  strcpy(buff, "write() with an invalid size:\n");
  if ( write(1, buff, -1) < 0 ) perror(); // Invalid size

  int counter = 0;
  while(1) {
    counter = addAsm(counter, 1);
    //itoa(gettime(), buff);
    //strcat(buff, "\n");
    //write(1, buff, strlen(buff));
  }
  
}
