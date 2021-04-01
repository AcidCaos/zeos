/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/** Screen functions **/
/**********************/

Byte inb (unsigned short port);
void clear ();
void scroll ();
void printc(char c);
void printc_error(char c);
void printc_color(char c, Byte fg_color, Byte bg_color);
void printc_xy(Byte x, Byte y, char c);
void printk(char *string);
void errork(char *string);


#endif  /* __IO_H__ */
