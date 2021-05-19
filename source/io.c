/*
 * io.c - Entrada/sortida per pantalla en mode sistema
 */

#include <io.h>
#include <types.h>
#include <cyclic_buffer.h>
#include <topbar.h>
#include <tty.h>

/**************/
/** Screen  ***/
/**************/

#define NUM_COLUMNS 80
#define NUM_ROWS    25

Byte x, y = 19;
Word *screen = (Word *) 0xb8000;

//*****************************//
// auxiliar (private) methods  //
//*****************************//

void print_to_bochs (char c) {
  // Magic BOCHS debug: writes 'c' to port 0xe9
  __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c)); 
}

void printc_attributes (char c, int fg_color, int bg_color, int blink) {
  
  if (c=='\n') {
    x = 0;
    if (y + 1 >= NUM_ROWS) scroll();
    else y = y + 1;
  }
  else {
    Word fg_attr = (Word) (fg_color & 0x000F);
    Word bg_attr = (Word) (bg_color & 0x0007) << 4;
    Word blink_attr = (Word) (blink & 0x0001) << 7;
    Word attr_byte = (fg_attr | bg_attr | blink_attr) << 8;
    Word ch = (Word) (c & 0x00FF) | attr_byte;

    screen[(y * NUM_COLUMNS + x)] = ch;
    if (++x >= NUM_COLUMNS) {
      x = 0;
      if (y + 1 >= NUM_ROWS) scroll();
      else y = y + 1;
    }
  }
}

//*****************************//
//   end of auxiliar methods   //
//*****************************//

/* Read a byte from 'port' */
Byte inb (unsigned short port) {
  Byte v;
  __asm__ __volatile__ ( "inb %w1, %0" : "=a" (v) : "Nd" (port) );
  return v;
}

void outb (unsigned char value, unsigned short port) {
  __asm__ __volatile__ ( "outb %0, %1" : : "a" (value), "Nd" (port) );
}

void clear () {
  Word ch = (Word) (' ' & 0x00FF);
  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLUMNS; col++) {
      screen[(row-1) * NUM_COLUMNS + col] = ch;
    }
  }
  x = 0;
  y = 0;
}

void scroll () { // if (topbar_enabled == 1) the TOP ROW should NOT be moved.
  // Move all up
  for (int row = 1 + topbar_enabled; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLUMNS; col++) {
      screen[(row-1) * NUM_COLUMNS + col] = screen[row * NUM_COLUMNS + col];
    }
  }
  // Clean last row
  Word ch = (Word) (' ' & 0x00FF);
  for (int col = 0; col < NUM_COLUMNS; col++) {
    screen[(NUM_ROWS-1) * NUM_COLUMNS + col] = ch;
  }
}

void printc (char c) {
  tty_printc(& ttys_table.ttys[ttys_table.focus], c);
  //printc_attributes(c, 0xF, 0x0, 0); // white on black
}

void printc_xy (Byte mx, Byte my, char c) {
  Byte cx = x, cy = y;
  x=mx; y=my;
  printc(c);
  x=cx; y=cy;
}

void printk (char *string) {

  //tty_printk (string); // Printk to tty device
  int i;
  for (i = 0; string[i]; i++) {
    print_to_bochs(string[i]);
    printc_attributes(string[i], 0xA, 0x0, 0); // green on black
  }
}

void printk_color_xy (char *string, Byte fg_color, Byte bg_color, Byte mx, Byte my) {
  int i;
  Byte cx = x, cy = y;
  x=mx; y=my;
  for (i = 0; string[i]; i++)
    printc_attributes(string[i], fg_color, bg_color, 0);
  x=cx; y=cy;
}
/*
void errork (char *string) {
  int i;
  for (i = 0; string[i]; i++)
    printc_error(string[i]); // light red on dark gray
}*/


