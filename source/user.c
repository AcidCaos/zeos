#include <libc.h>
#include <adam.h>

void print (char * s);

void terminal_codes();

int __attribute__ ((__section__(".text.main"))) main(void) {
  
  //while (1); 
  
  adam(); // This is the adam shell. Check out 'adam' folder.

  exit();

  return 0; // Never reached
}
