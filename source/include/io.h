/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/** Screen functions **/
/**********************/

Byte inb (unsigned short port);
void print_to_bochs (char c);

void clear ();
void scroll ();
void set_cursor (int xx, int yy);

void printc(char c);
void printc_error(char c);
void printc_color(char c, Byte fg_color, Byte bg_color);
void printc_color_xy (char c, Byte fg_color, Byte bg_color, Byte mx, Byte my);
void printc_xy(Byte x, Byte y, char c);

void print_text_cursor ();

void printk(char *string);
void printk_color_xy(char *string, Byte fg_color, Byte bg_color, Byte mx, Byte my);
void errork(char *string);


#endif  /* __IO_H__ */
