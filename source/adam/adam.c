#include <libc.h>
#include <adam.h>
#include <adamlib.h>

int adam () {
  
  char head[] = "adam@zeos> ";
  char buffer[1024];  
  
  print("\nAdam [Version 1.0]\n");
  print("A simple shell in ZeOS, the father of all processes.\n");
  print("Enter 'h' for help.\n\n");
  
  while (1) {
    
    print(head);
    input(buffer); // Reads until [ENTER] is hit.
    
    execute(buffer);
    
  }
  return 0;
}


int execute(char * command) {
  
  if      (strequ(command, "h")) help();
  else if (strequ(command, "ping")) print("pong\n");
  else    {
    print("Command '"); print(command); print("' does not exist.\n");
  }
}

void help() {
  print("Adam is a simple shell in ZeOS.\n");
  print("List of accepted commands:\n\n");
  print("    h  Shows this help message.\n");
  print(" ping  Answers 'pong'.\n");
  print("\n");
}
