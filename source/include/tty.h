#ifndef __TTY_H__
#define __TTY_H__


#define NUM_COLUMNS 80
#define NUM_ROWS    25

#define MAX_TTYS    30

#include <types.h>

/*
    0 - black          8 - dark gray
    1 - blue           9 - light blue
    2 - green          A - light green
    3 - cyan           B - light cyan
    4 - red            C - light red
    5 - magenta        D - light magenta
    6 - brown          E - yellow
    7 - light gray     F - white

    background : 0..7
    foreground : 0..F
*/

struct tty {
  int x, y; // Cursor position and color
  int current_fg_color, current_bg_color; // Current colors
  int current_blinking; // Current character blinks
  Word buffer [NUM_COLUMNS * NUM_ROWS]; // Screen buffer
  int pid_maker;
  
};

struct ttys_table {
  int use_count [MAX_TTYS];
  //struct tty* ttys [MAX_TTYS]; // TODO : hauria de ser punters a direccions de memoria d.usuari protegides
  struct tty ttys [MAX_TTYS];
  int focus;
  
};

struct ttys_table ttys_table;


void init_tty (struct tty* tty);
void init_ttys_table();
struct tty* get_init_free_tty ();
int increment_use_count_tty (struct tty* tty);
int decrement_use_count_tty (struct tty* tty);

int sys_write_console(void* device, char *buffer, int size);
//int sys_write_console_error(char *buffer, int size);

void set_current_cursor (int x, int y);
void set_current_bg_color (int c);
void set_current_fg_color (int c);
void set_current_general_attr (int n);

int show_next_tty ();
int force_show_tty (int i);

void show_console ();

void tty_printc (struct tty* tty, char c);
void tty_printk (char* str);
void tty_scroll (struct tty* tty);



#endif
