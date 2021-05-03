/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/** Screen functions **/
/**********************/

Byte inb (unsigned short port);
void outb (unsigned char value, unsigned short port);

void print_to_bochs (char c);

void clear ();
void scroll ();
void set_cursor (int xx, int yy);

void printc(char c);
void printc_xy(Byte x, Byte y, char c);

void printk(char *string);
void printk_color_xy(char *string, Byte fg_color, Byte bg_color, Byte mx, Byte my);



#endif  /* __IO_H__ */
