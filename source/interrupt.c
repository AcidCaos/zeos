/*
 * interrupt.c - Definició de les diferents rutines de tractament d'exepcions
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>
#include <zeos_interrupt.h>
#include <ticks.h>




Gate idt[IDT_ENTRIES];
Register    idtR;

char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','¡','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','ñ',
  '\0','º','\0','ç','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}



// # include del header de entry.S
void clock_handler();
void keyboard_handler();

void writeMsr(int msr, int data);
void syscall_handler_sysenter();

void syscall_handler();





void setIdt() {

  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  

  // EXCEPTION HANDLERS (zeos_interrupt.h)
  set_handlers();


  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */
  
  setInterruptHandler(32, clock_handler, 0); // clock_handler
  setInterruptHandler(33, keyboard_handler, 0); // keyboard handler

  // syscall_handler with SYSENTER.
  writeMsr(0x174, __KERNEL_CS);
  writeMsr(0x175, INITIAL_ESP);
  writeMsr(0x176, syscall_handler_sysenter);

  // syscall_handler with INT.   user privilege level = 3
  setTrapHandler(0x80, syscall_handler, 3);


  set_idt_reg(&idtR);
}


 /* HARDWARE INTERRUPT ROUTINES */

void clock_routine() {
  zeos_ticks++;
  zeos_show_clock();
  return;
}

void keyboard_routine() {
  unsigned char p = inb(0x60); // Keyboard data register port = 0x60
  char mkbrk = (p >> 7) & 0x01;
  char scancode = p & 0x7F;
  if (mkbrk == 0x00){ //Make
    char pr = char_map[scancode];
    if (pr == '\0') pr = 'C';
    printc_xy(0, 0, pr);
    char str[] = "Pressed:  \n";
    str[9] = pr;
    printk(&str);
  }
  else if (mkbrk == 0x01){ //Break
    
  } else {
    printk("keyboard_routine(): This should never happen!");
  }
  return;
}









