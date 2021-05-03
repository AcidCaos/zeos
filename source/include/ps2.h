
#ifndef __PS2_H__
#define __PS2_H__

struct ps2_mouse {
  
  unsigned int read_x; // Raw read values
  unsigned int read_y;
  
  unsigned int lineal_x; // Linearized Values
  unsigned int lineal_y;
  
  unsigned int lineal_x_max; // Max values for Linearized
  unsigned int lineal_y_max;
  
  unsigned int x; // Display Normalized Values
  unsigned int y;
  
};

struct ps2_mouse mouse;

void ps2_mouse_init ();
void ps2_mouse_routine_inner ();
void show_mouse ();

#endif

