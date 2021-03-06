#include <libc.h>

char buff[24];

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
  
  
  int counter = 0;
  while(1) {
    counter = addAsm(counter, 1);
    char* buffer = "User write()";
    write(1, buffer, strlen(buffer));
  }
  
}
