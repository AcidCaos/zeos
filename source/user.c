#include <libc.h>
#include <adam.h>

void print (char * s);
void pstats (struct stats* s);
void pstatsline (struct stats* s);


int __attribute__ ((__section__(".text.main"))) main(void) {
  
  adam(); // This is the adam shell. Check out 'adam' folder.

  print("Adam ended.\n");

  exit();

  return 0; // Never reached
}


