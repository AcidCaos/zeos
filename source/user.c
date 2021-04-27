#include <libc.h>
#include <adam.h>

void print (char * s);

void terminal_codes();

int __attribute__ ((__section__(".text.main"))) main(void) {
  
  
  int newfd = createScreen();
  write (newfd, "New", 3);
  
  // terminal_codes();

  adam(); // This is the adam shell. Check out 'adam' folder.

  print("Adam ended.\n");

  exit();

  return 0; // Never reached
}

void terminal_codes() {
  
  // canvi posicio cursor
  print(">>\033 \x1b \n");
  print("[ 55>>\033 [ 55 \n");
  print("[ 55 ; >>\033 [ 55 ;\n");
  print("[ 5 ; 6 >>\033 [ 5 ; 6  \n");
  print("[ 5 ; 6 a >>\033 [ 5 ; 6 a  \n");
  print("!! [ 5 ; 6 f >>\033 [ 5 ; 6 ff  \n");
  print("!! [5;6H>>\033[5;6H \n");
  // general text attr
  print("[ 5 ; 6 >>\033 [ 5 ; 6 \n");
  print("!! [ 5 m; 6 >>\033 [ 5 m; 6  \n");
  print("!! [ 5m ; 6 >>\033 [ 5m ; 6  \n");
  print("[ m5 ; 6 >>\033 [ m5 ; 6  \n");
  print("[ ;m5 ; 6 >>\033 [ ;m5 ; 6  \n");
  // canvi color
  print("[ 4 ; 6 >>\033 [ 4 ; 6  \n");
  print("!! [ 45 m; 6 >>\033 [ 45 m; 6  \n");
  print("!! [ 35m ; 6 >>\033 [ 35m ; 6  \n");
  print("!! [ 4 5 m; 6 >>\033 [ 4 5 m; 6  \n");
  print("[ 3 5; 6 >>\033 [ 3 5; 6  \n");
  print("[  4;;5 m; 6 >>\033 [  4;;5 m; 6  \n");
  print("[ ;35m ; >>\033 [ ;35m ;  \n");
  print("[ m5 ; 6 >>\033 [ m5 ; 6  \n");
  print("[ ;m5 ; 6 >>\033 [ ;m5 ; 6  \n");
  // TODO : borrar caràcter actual
  
  
}
