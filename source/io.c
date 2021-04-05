/*
 * io.c - Entrada/sortida per pantalla en mode sistema
 */

#include <io.h>
#include <types.h>
#include <cyclic_buffer.h>

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
  print_to_bochs(c);
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
  __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
  return v;
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

void scroll () {
  // Move all up
  for (int row = 1; row < NUM_ROWS; row++) {
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
  printc_attributes(c, 0xF, 0x0, 0); // white on black
}

void printc_error (char c) {
  printc_attributes(c, 0xC, 0x0, 0); // light red on black
}

void printc_color (char c, Byte fg_color, Byte bg_color) {
  printc_attributes(c, fg_color, bg_color, 0);
}

void printc_xy (Byte mx, Byte my, char c) {
  Byte cx = x, cy = y;
  x=mx; y=my;
  printc(c);
  x=cx; y=cy;
}

void printk (char *string) {
  int i;
  for (i = 0; string[i]; i++)
    printc_attributes(string[i], 0xA, 0x0, 0); // green on black
}

void errork (char *string) {
  int i;
  for (i = 0; string[i]; i++)
    printc_error(string[i]); // light red on dark gray
}


