#include <tty.h>
#include <io.h>


// struct ttys_table ttys_table;

void set_current_cursor (int x, int y) {
  printk("CURSOR");
  printk(" -> x=");
  char aux[] = "_"; aux[0] = '0' + x;
  printk(aux);
  printk(" ; y=");
  char aux2[] = "_"; aux2[0] = '0' + y;
  printk(aux2);
}

void set_current_bg_color (int c) {
  printk("BG_COLOR");
  printk(" -> color=");
  char aux[] = "_"; aux[0] = '0' + (char) c;
  printk(aux);
}

void set_current_fg_color (int c) {
  printk("FG_COLOR");
  printk(" -> color=");
  char aux[] = "_"; aux[0] = '0' + (char) c;
  printk(aux);
}

void set_current_general_attr (int n) {
  printk("GENERAL_ATTR");
  printk(" -> n=");
  char aux[] = "_"; aux[0] = '0' + n;
  printk(aux);
}


int show_next_tty () {// Shift +TAB pressed (called in interrupt.c)
  printk("[Show next tty]\n");
  return 0;
}
