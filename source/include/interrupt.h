/*
 * interrupt.h - Definició de les diferents rutines de tractament d'exepcions
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <types.h>

#define IDT_ENTRIES 256

char char_map[98];
char key_is_pressed [256];

extern Gate idt[IDT_ENTRIES];
extern Register idtR;

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL);
void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL);

void setIdt();


void clock_routine();

void keyboard_routine();

void ps2_mouse_routine();

// PS/2 Mouse
void ps2_mouse_init ();

#endif  /* __INTERRUPT_H__ */
