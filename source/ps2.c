#include <ps2.h>
#include <io.h>

#define NUM_COLUMNS 80
#define NUM_ROWS    25

int button_middle_pressed;
int button_right_pressed;
int button_left_pressed;


//
//  PS/2 Auxiliar Driver Functions
//

void ps2_wait_input_buffer_empty () { // Type 1
  // printk("    - Wait Input Register Empty.");
  unsigned int time_out = 100000;
  while (time_out--) {
    if ((inb(0x64) & 2) == 0) {/*printk(" Ok. Empty: can send new commands!\n");*/ return;}
    //if (time_out > 100000 - 20) printk(".");
  }
  printk("Error: time-out for Poll bit 1.\n");
}

void ps2_wait_output_buffer_full () { // Type 0
  // printk("    - Wait Output Register Full.");
  unsigned int time_out = 100000;
  while (time_out--) {
    if ((inb(0x64) & 1) == 1) {/*printk(" Ok. Full: data available!\n");*/ return;}
    //if (time_out > 100000 - 20) printk(".");
  }
  printk("Error: time-out for Poll bit 0.\n");
}

//
//  PS/2 Driver Functions
//


// Sending commands to KBC
void ps2_kbc_send_command (unsigned char command) {
  ps2_wait_input_buffer_empty ();
  outb (command, 0x64);
}

void ps2_kbc_send_argument (unsigned char argument) {
  ps2_wait_input_buffer_empty ();
  outb (argument, 0x60);
}

unsigned char ps2_read_buffer () {
  ps2_wait_output_buffer_full ();
  return inb(0x60);
}

void ps2_mouse_send_comand (unsigned char command) { // Send command to mouse (not KBC)
  unsigned char reply;
  
  ps2_kbc_send_command (0xD4); // Write Byte to Mouse
  ps2_kbc_send_argument (command);
  
  reply = ps2_read_buffer();
  /*if (reply == 0xFA) printk("    ACK 1\n");
  else */if (reply == 0xFE || reply == 0xFC) printk("ERROR: Mouse did not ACK.\n");
  
}


//
//  PS/2 Mouse Functions
//

void ps2_mouse_init () {
  unsigned char status_byte;
  
  
  // printk("* PS/2 KBC Command: Enable Mouse\n");
  ps2_kbc_send_command (0xA8); // Enable Mouse
  
  
  // printk("* PS/2 KBC Command: Read Command Byte\n");
  ps2_kbc_send_command (0x20); // Read Command Byte
  status_byte = ps2_read_buffer();
  
  
  status_byte = status_byte | 2;     // (set   bit 2) Enable IRQ-12 : interrupt from mouse.
  //status_byte = status_byte & ~0x20; // (clear bit 5) Disable Mouse Clock.
  
  
  // printk("* PS/2 KBC Command: Write Command Byte (Enabled interrupt from mouse)\n");
  ps2_kbc_send_command (0x60); // Write Command Byte
  ps2_kbc_send_argument (status_byte);
  
  /* Check */
  
  // printk("* PS/2 KBC Command: Read Command Byte\n");
  ps2_kbc_send_command (0x20); // Read Command Byte
  unsigned char aux = ps2_read_buffer();
  
  if (status_byte != aux) printk("ERROR: Mouse KBC Command: 'Write Command Byte' Failed.\n");
  
  /* end */
  
  // printk("* PS/2 Mouse Command: Set Default values\n");
  ps2_mouse_send_comand (0xF6); // Set Default values
  
  
  // printk("* PS/2 Mouse Command: Enable Data Reporting\n");
  ps2_mouse_send_comand (0xF4); // Enable Data Reporting (In stream mode only)
  
  /*
  // printk("* PS/2 Mouse Command: Enable Data Reporting\n");
  ps2_mouse_send_comand (0xF4); // Enable Data Reporting (In stream mode only)
  */
  
  button_middle_pressed = 0;
  button_right_pressed = 0;
  button_left_pressed = 0;
  
}

//
//  PS/2 Mouse Interrupt Routine
//
  
void ps2_mouse_routine_inner () {
  
  unsigned char status = ps2_read_buffer();
  int xm = 0x0FF & (int) ps2_read_buffer();
  int ym = 0x0FF & (int) ps2_read_buffer();
  
  /*
   STATUS Byte:
   
   yo xo ys xs ao bm br bl
   
   Y-Overflow
   X-overflow
   
   Y-sign
   X-sign
   
   Always One
   
   Button Middle (Off = 0)
   Button Right
   Button Left
  */
  
  int y_overflow = (status >> 7) & 0x1;
  int x_overflow = (status >> 6) & 0x1;
  
  int y_sign = (status >> 5) & 0x1;
  int x_sign = (status >> 4) & 0x1;
  
  int always_one = (status >> 3) & 0x1;
  
  int button_middle = (status >> 2) & 0x1;
  int button_right = (status >> 1) & 0x1;
  int button_left = status & 0x1;
  
  
  // Buttons
  
  if (button_middle && ! button_middle_pressed) { button_middle_pressed = 1; return; }
  if (button_right && ! button_right_pressed) { button_right_pressed = 1; return; }
  if (button_left && ! button_left_pressed) { button_left_pressed = 1; return; }
  
  if (! button_middle && button_middle_pressed) { button_middle_pressed = 0; return; }
  if (! button_right && button_right_pressed) { button_right_pressed = 0; return; }
  if (! button_left && button_left_pressed) { button_left_pressed = 0; return; }
  
  
  // Calculate movement
  
  mouse.read_x = xm;
  mouse.read_y = ym;
  
  mouse.lineal_x = ( x_sign ? (xm) : (256 + xm) ) - 76;
  mouse.lineal_y = 256 - ( y_sign ? (ym) : (256 + ym) ) + 100;
  //mouse.lineal_y = ( y_sign ? (ym) : (256 + ym) ) - 132; // Inverted Y-axis
  
  mouse.lineal_x_max = 256 + 179 - 76; // 359
  mouse.lineal_y_max = 256 + 100 - 132; // 224
  
  mouse.x = (mouse.lineal_x * NUM_COLUMNS) / mouse.lineal_x_max;
  mouse.y = (mouse.lineal_y * NUM_ROWS) / mouse.lineal_y_max;
  
}


void show_mouse () {
  printk_color_xy ("o", 0xF, 0x0, (Byte) mouse.x, (Byte) mouse.y);
}


