#include <topbar.h>
#include <io.h>
#include <sched.h>
#include <interrupt.h>
#include <tty.h>

char* strcpy(char* d, const char* s);
char* strcat(char* str1, const char* str2);
void itoa(int a, char *b);
int strlen(char *a);

char fg_color = 0xE; // yellow
char bg_color = 0x1; // blue

char last_key_pressed[32];
int fps;

char buffer[128];
char aux[32];

// Num. Cols = 80; at io.c: 
#define NUM_COLUMNS 80


void init_topbar() {
  topbar_enabled = 1;
  // Fill the row
  for (int x = 0; x<80; x+=10) printk_color_xy("          ", fg_color, bg_color, x, 0);
}

int frame_counter;
int last_time;
#include <utils.h> // for get_ticks()
void track_fps() {
  frame_counter++;
  int now = get_ticks();
  if (now - last_time >= 1000) { // milisecs.?
    last_time = now;
    fps = frame_counter;
    frame_counter = 0;
  }
}

void update_topbar() {
  
  if (!topbar_enabled) return;
  
  track_fps();
  
  // Fill the row
  //for (int x = 0; x<80; x+=10) printk_color_xy("          ", fg_color, bg_color, x, 0);
  
  // Print Head
  printk_color_xy("ZeOS", fg_color, bg_color, 0, 0);
  
  // Print running process
  if(current()->PID == 0)
    printk_color_xy("CPU: IDLE        ", fg_color, bg_color, 8, 0); // Spaces: Ensure erasing previous value
  else {
    printk_color_xy("CPU: PID         ", fg_color, bg_color, 8, 0);
    itoa(current()->PID, buffer);
    printk_color_xy(buffer, fg_color, bg_color, 17, 0);
  }
  
  // Print tty info
  printk_color_xy("TTY", fg_color, bg_color, 37, 0);
  itoa(ttys_table.focus, buffer);
  printk_color_xy(buffer, fg_color, bg_color, 41, 0);
  
  printk_color_xy("(PID", fg_color, bg_color, 44, 0);
  itoa(ttys_table.ttys[ttys_table.focus].pid_maker, buffer); // maker's pid
  strcat (buffer, "; ");
  
  itoa(fps, aux);
  strcat (buffer, aux);
  strcat (buffer, " FPS)");
  strcat (buffer, "      "); // Ensure erasing previous values
  printk_color_xy(buffer, fg_color, bg_color, 49, 0);
  
  // Print Last pressed key
  strcpy (buffer, "             "); // Ensure erasing previous values
  strcpy (buffer + strlen(buffer) - strlen(last_key_pressed), last_key_pressed);
  printk_color_xy(buffer, fg_color, bg_color, NUM_COLUMNS - strlen(buffer), 0);
  
}

void update_last_key_pressed() { // Called at Keyboard Interrupt
  
  // ASCII Text key
  char ascii_key = 0;
  int SPACE = key_is_pressed [0x39];
  int ENTER = key_is_pressed [0x1C];
  
  // Control keys
  int ESC = key_is_pressed [0x01];
  int TAB = key_is_pressed [0x0F];
  int SHIFT_L = key_is_pressed [0x2A];
  int SHIFT_R = key_is_pressed [0x36];
  int CTRL_L = key_is_pressed [0x1D];
  int CTRL_R = key_is_pressed [0x46];
  int ALT = key_is_pressed [0x38];
  int UP = key_is_pressed [0x48];
  int DOWN = key_is_pressed [0x50];
  int LEFT = key_is_pressed [0x4B];
  int RIGHT = key_is_pressed [0x4D];
  int DEL = key_is_pressed [0x53];
  int RET = key_is_pressed [0x0E];
  
  int control = ESC || TAB || SHIFT_L || SHIFT_R || CTRL_L || CTRL_R || ALT || UP || DOWN || LEFT || RIGHT || DEL || RET;
  
  
  for (int p = 0; p < 256; p++) {
    if (key_is_pressed [p] && char_map[p] != 0) ascii_key = char_map[p];
  }
  
  if ( ! control ) {
    if      (SPACE) strcpy(last_key_pressed, "[SPACE]");
    else if (ENTER) strcpy(last_key_pressed, "[ENTER]");
    else {
      last_key_pressed[0] = ascii_key;
      last_key_pressed[1] = 0;
    }
  }
  else {
    if (TAB && (SHIFT_L || SHIFT_R)) strcpy(last_key_pressed, "[SHIFT+TAB]");
    else if (TAB) strcpy(last_key_pressed, "[TAB]");
    else if (SHIFT_L || SHIFT_R) {
      if (ascii_key >= 'a' && ascii_key <= 'z'){ // Shift keys + lowercase
        ascii_key = ascii_key - 'a' + 'A';
        last_key_pressed[0] = ascii_key;
        last_key_pressed[1] = 0;
      }
      else strcpy(last_key_pressed, "[SHIFT]");
    }
    else if (CTRL_L || CTRL_R) strcpy(last_key_pressed, "[CTRL]");
    else if (ALT) strcpy(last_key_pressed, "[ALT]");
    else if (UP) strcpy(last_key_pressed, "[UP]");
    else if (DOWN) strcpy(last_key_pressed, "[DOWN]");
    else if (LEFT) strcpy(last_key_pressed, "[LEFT]");
    else if (RIGHT) strcpy(last_key_pressed, "[RIGHT]");
    else if (DEL) strcpy(last_key_pressed, "[DEL]");
    else if (RET) strcpy(last_key_pressed, "[RET]");
    else if (ESC) strcpy(last_key_pressed, "[ESC]");
  }
}

int strlen(char *a) {
  int i;
  i=0;
  while (a[i]!=0) i++;
  return i;
}

char* strcpy(char* d, const char* s) {
  if (d == NULL) return NULL; 
  char* ptr = d;
  while (*s != 0) {
    *d = *s;
    d++;
    s++;
  }
  *d = '\0';
  return ptr;
}

char* strcat(char* str1, const char* str2) {
  char* ret = str1;
  while (*str1) str1++;
  while ((*str1++ = *str2++));
  return ret;
}


