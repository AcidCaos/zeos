/*
 * interrupt.c - Definició de les diferents rutines de tractament d'exepcions
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>
#include <tty.h>
#include <topbar.h>
#include <zeos_interrupt.h>
#include <ticks.h>
#include <sched.h>

#include <list.h>
#include <devices.h>
#include <cyclic_buffer.h>
#include <ps2.h>



Gate idt[IDT_ENTRIES];
Register    idtR;

char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','¡','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\n','\0','a','s',
  'd','f','g','h','j','k','l','ñ',
  '\0','º','\0','ç','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0',' ','\0','\0','\0','\0','\0','\0',
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
void ps2_mouse_handler();

void writeMsr(int msr, int data);
void syscall_handler_sysenter();

void syscall_handler();





void setIdt() {

  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  

  // EXCEPTION HANDLERS (zeos_interrupt.h)
  set_handlers();


  
  // Default Interrupt Vector Assignment
  /*
      INT     Descr.
      0-31    Protected Mode Exceptions (Reserved by Intel)
      8-15    Default mapping of IRQ0-7 by BIOS at Bootstrap
      70h-78h Default mapping of IRQ8-15 by BIOS at Bootstrap
  */
  
  // Inicialize Interrupt Vector
  setInterruptHandler(32, clock_handler, 0); // clock_handler       IRQ-0
  setInterruptHandler(33, keyboard_handler, 0); // keyboard handler IRQ-1
  setInterruptHandler(44, ps2_mouse_handler, 0); // PS/2 mouse handler IRQ-12

  
  // syscall_handler with SYSENTER.
  writeMsr(0x174, __KERNEL_CS);
  writeMsr(0x175, INITIAL_ESP);
  writeMsr(0x176, (int) syscall_handler_sysenter);

  // syscall_handler with INT.   user privilege level = 3
  setTrapHandler(0x80, syscall_handler, 3);


  set_idt_reg(&idtR);
}


 /* HARDWARE INTERRUPT ROUTINES */

void clock_routine() {
  zeos_ticks++;
  
  if (zeos_ticks > 1) show_console(); // Da fak

  update_topbar();
  
  show_mouse();
  
  scheduler();
  
  return;
}


// extern struct cyclic_buffer console_input; // devices.h


void keyboard_routine() {
  
  unsigned char p = inb(0x60); // Keyboard data register port = 0x60
  unsigned char mkbrk = (p >> 7) & 0x01;
  unsigned char scancode = p & 0x7F;

  //printk(" --> keyboard_routine()\n");
  
  if (mkbrk == 0x00){ //Make : key pressed

    char pr = char_map[scancode];
    key_is_pressed [scancode] = 1;

    int TAB = key_is_pressed [0x0F];
    int SHIFT_L = key_is_pressed [0x2A];
    int SHIFT_R = key_is_pressed [0x36];
    
    if (pr == '\0') {
      
      pr = '#';
      
    }
    else { // is a printable character
      if (pr == '\n'){ // to delete the writting cursor
        printc(' ');
      }
      if ((SHIFT_L || SHIFT_R) && (pr >= 'a' && pr <= 'z')) // Shift keys + lowercase
        pr = pr - 'a' + 'A';
        
      //printc_color(pr, 0xE, 0x0);
      printc(pr);
      
      // Reading buffer
      push_to_focus_read_buffer (pr);
      
    }
    
    // Topbar update last key pressed
    update_last_key_pressed ();
    
    // Check if Change of tty needed
    if ((SHIFT_L || SHIFT_R) && TAB) 
	show_next_tty ();


  } else if (mkbrk == 0x01){ //Break: key unpressed
    key_is_pressed [scancode] = 0;
  } else {
    printk(" --> keyboard_routine() : This should never happen!\n");
  }
  //printk(" --> keyboard_routine() EOF\n");
  return;
}

#include <ps2.h>

void ps2_mouse_routine () {
  
  ps2_mouse_routine_inner ();
  
}


