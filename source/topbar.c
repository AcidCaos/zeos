#include <topbar>

  char fg_color;
  char bg_color;
  char head[];
  char idle[];
  char run[];
  char totalprocs[];
  char lastkey[];

void init_topbar() { // TODO 
  
  fg_color = 0xE; // yellow
  bg_color = 0x1; // blue
  
  head[] = "ZeOS";
  idle[] = "CPU IDLE";
  run[] = "RUNNING PID "; // 200
  totalprocs[] = "Total procs ";
  lastkey[] = "[SPACE]";
  
}

void update() {
  
  
}
