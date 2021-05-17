#include <topbar.h>
#include <io.h>
#include <sched.h>
#include <interrupt.h>
#include <tty.h>
#include <ps2.h>

char* strcpy_k(char* d, const char* s);
char* strcat_k(char* str1, const char* str2);
void itoa_k(int a, char *b);
int strlen_k(char *a);

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


#include <utils.h> // for get_ticks()

int frame_counter;
unsigned long long last_time;

#define IPS 4000000

char buff[256];

void track_fps() {
  frame_counter++;
  unsigned long long now = get_ticks();
  
  /*itoa_k(now, buff);
  printk(buff);
  printk("\n");*/

  if (now - last_time >= IPS) {
    
    // printk("Second\n");
    
    last_time = now;
    fps = frame_counter;
    frame_counter = 0;
  }
}

void update_topbar() {
  
  if (!topbar_enabled) return;
  
  track_fps();
  
  // Print Head
  printk_color_xy("ZeOS    ", fg_color, bg_color, 0, 0);
  
  // Print running process
  if(current()->PID == 0)
    printk_color_xy("CPU: IDLE        ", fg_color, bg_color, 8, 0); // Spaces: Ensure erasing previous value
  else {
    printk_color_xy("CPU: PID         ", fg_color, bg_color, 8, 0);
    itoa_k(current()->PID, buffer);
    printk_color_xy(buffer, fg_color, bg_color, 17, 0);
  }
  
  // Print tty info
  printk_color_xy(" (PID ", fg_color, bg_color, 30, 0); //44
  itoa_k(ttys_table.ttys[ttys_table.focus].pid_maker, buffer); // maker's pid
  strcat_k (buffer, "; ");
  
  // Print tty info
  printk_color_xy("TTY ", fg_color, bg_color, 24, 0); //37
  itoa_k(ttys_table.focus, buffer);
  strcat_k (buffer, " ");
  printk_color_xy(buffer, fg_color, bg_color, 28, 0); //41
  
  itoa_k(fps, aux);
  strcat_k (buffer, "; ");
  strcat_k (buffer, aux);
  strcat_k (buffer, " FPS)");
  strcat_k (buffer, "       "); // Ensure erasing previous values
  printk_color_xy(buffer, fg_color, bg_color, 36, 0); //49
  
  // Mous position
  int xcoord = mouse.x;
  int ycoord = mouse.y;
  
  strcpy_k (buffer, "    (");
  itoa_k(xcoord, aux);
  strcat_k (buffer, aux);
  strcat_k (buffer, ", ");
  itoa_k(ycoord, aux);
  strcat_k (buffer, aux);
  strcat_k (buffer, ")  ");
  printk_color_xy (buffer, fg_color, bg_color, 53, 0); //24
  
  // Print Last pressed key
  strcpy_k (buffer, "              "); // Ensure erasing previous values
  strcpy_k (buffer + strlen_k(buffer) - strlen_k(last_key_pressed), last_key_pressed);
  printk_color_xy(buffer, fg_color, bg_color, NUM_COLUMNS - strlen_k(buffer), 0);
  
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
    if      (SPACE) strcpy_k(last_key_pressed, "[SPACE]");
    else if (ENTER) strcpy_k(last_key_pressed, "[ENTER]");
    else {
      last_key_pressed[0] = ascii_key;
      last_key_pressed[1] = 0;
    }
  }
  else {
    if (TAB && (SHIFT_L || SHIFT_R)) strcpy_k(last_key_pressed, "[SHIFT+TAB]");
    else if (TAB) strcpy_k(last_key_pressed, "[TAB]");
    else if (SHIFT_L || SHIFT_R) {
      if (ascii_key >= 'a' && ascii_key <= 'z'){ // Shift keys + lowercase
        ascii_key = ascii_key - 'a' + 'A';
        last_key_pressed[0] = ascii_key;
        last_key_pressed[1] = 0;
      }
      else strcpy_k(last_key_pressed, "[SHIFT]");
    }
    else if (CTRL_L || CTRL_R) strcpy_k(last_key_pressed, "[CTRL]");
    else if (ALT) strcpy_k(last_key_pressed, "[ALT]");
    else if (UP) strcpy_k(last_key_pressed, "[UP]");
    else if (DOWN) strcpy_k(last_key_pressed, "[DOWN]");
    else if (LEFT) strcpy_k(last_key_pressed, "[LEFT]");
    else if (RIGHT) strcpy_k(last_key_pressed, "[RIGHT]");
    else if (DEL) strcpy_k(last_key_pressed, "[DEL]");
    else if (RET) strcpy_k(last_key_pressed, "[RET]");
    else if (ESC) strcpy_k(last_key_pressed, "[ESC]");
  }
}

int strlen_k(char *a) {
  int i;
  i=0;
  while (a[i]!=0) i++;
  return i;
}

char* strcpy_k(char* d, const char* s) {
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

char* strcat_k(char* str1, const char* str2) {
  char* ret = str1;
  while (*str1) str1++;
  while ((*str1++ = *str2++));
  return ret;
}

void itoa_k(int a, char *b) {
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  
  if (a < 0) { b[0]='-'; b++; a = - a;}
  
  while (a>0) {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++) {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

